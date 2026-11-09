param(
    [Parameter(Mandatory = $true)]
    [string]$VariantName,

    [Parameter(Mandatory = $true)]
    [string]$ExePath,

    [Parameter(Mandatory = $true)]
    [string]$InputRoot,

    [Parameter(Mandatory = $true)]
    [string]$OutputCsv,

    [ValidateSet('vector', 'list', 'deque')]
    [string]$Container = 'vector',

    [ValidateSet(1, 2)]
    [int]$Strategy = 1,

    [ValidateSet(1, 2, 3)]
    [int]$BenchmarkMode = 1,

    [ValidateSet(1, 2)]
    [int]$SortChoice = 1
)

$files = @(1000, 10000, 100000, 1000000, 10000000)
$results = @()

function Invoke-BenchmarkRun {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Executable,

        [Parameter(Mandatory = $true)]
        [string]$BenchmarkRoot,

        [Parameter(Mandatory = $true)]
        [int]$RecordCount
    )

    $inputPath = Join-Path $BenchmarkRoot ("generated\students_{0}.txt" -f $RecordCount)
    $containerChoice = switch ($Container) {
        'vector' { 1 }
        'list' { 2 }
        default { 3 }
    }
    $psi = [System.Diagnostics.ProcessStartInfo]::new()
    $psi.FileName = $Executable
    $psi.WorkingDirectory = $BenchmarkRoot
    $psi.RedirectStandardInput = $true
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.UseShellExecute = $false
    $psi.CreateNoWindow = $true

    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $psi

    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $state = [hashtable]::Synchronized(@{ PeakMB = 0.0 })
    $timer = [System.Timers.Timer]::new(1000)
    $timer.AutoReset = $true
    $handler = [System.Timers.ElapsedEventHandler]{
        param($sender, $eventArgs)
        try {
            $sample = Get-Process -Id $process.Id -ErrorAction Stop
            $workingSetMB = [math]::Round($sample.WorkingSet64 / 1MB, 2)
            if ($workingSetMB -gt $state.PeakMB) {
                $state.PeakMB = $workingSetMB
            }
        } catch {
        }
    }.GetNewClosure()
    $timer.add_Elapsed($handler)
    $timer.Start()

    $stdout = ''
    $stderr = ''
    $exitCode = $null
    try {
        [void]$process.Start()
        $process.StandardInput.WriteLine('3')
        $process.StandardInput.WriteLine('A')
        $process.StandardInput.WriteLine($inputPath)
        $process.StandardInput.WriteLine([string]$SortChoice)
        $process.StandardInput.WriteLine([string]$BenchmarkMode)

        if ($BenchmarkMode -eq 1) {
            $process.StandardInput.WriteLine([string]$containerChoice)
            $process.StandardInput.WriteLine([string]$Strategy)
        } elseif ($BenchmarkMode -eq 2) {
            $process.StandardInput.WriteLine([string]$Strategy)
        }

        $process.StandardInput.Close()

        while (-not $process.WaitForExit(200)) {
            try {
                $sample = Get-Process -Id $process.Id -ErrorAction Stop
                $workingSetMB = [math]::Round($sample.WorkingSet64 / 1MB, 2)
                if ($workingSetMB -gt $state.PeakMB) {
                    $state.PeakMB = $workingSetMB
                }
            } catch {
            }
        }

        try {
            $sample = Get-Process -Id $process.Id -ErrorAction Stop
            $workingSetMB = [math]::Round($sample.WorkingSet64 / 1MB, 2)
            if ($workingSetMB -gt $state.PeakMB) {
                $state.PeakMB = $workingSetMB
            }
        } catch {
        }

        $stdout = $process.StandardOutput.ReadToEnd()
        $stderr = $process.StandardError.ReadToEnd()
        $exitCode = $process.ExitCode
    } finally {
        $stopwatch.Stop()
        $timer.Stop()
        $timer.Dispose()
        if ($process -and -not $process.HasExited) {
            try {
                $process.Kill()
            } catch {
            }
        }
        if ($process) {
            $process.Dispose()
        }
    }

    $result = [ordered]@{
        Variant = $VariantName
        Container = $Container
        Strategy = $Strategy
        Size = $RecordCount
        InputFile = $inputPath
        PassedCount = ''
        FailedCount = ''
        Reading_us = ''
        Sorting_us = ''
        Splitting_us = ''
        WritingPassed_us = ''
        WritingFailed_us = ''
        Total_us = [int64]($stopwatch.Elapsed.Ticks / 10)
        PeakWorkingSetMB = $state.PeakMB
        Status = 'ok'
        Error = ''
    }

    if ($exitCode -ne 0) {
        $result.Status = 'failed'
        $result.Error = "Exit code ${exitCode}: $stderr"
        return [pscustomobject]$result
    }

    $pattern = 'Passed file:\s+.+\((?<passed>\d+) students\).*?Failed file:\s+.+\((?<failed>\d+) students\).*?Phase timings \(microseconds\):\s*Reading:\s+(?<reading>\d+)\s*Sorting:\s+(?<sorting>\d+)\s*Splitting:\s+(?<splitting>\d+)\s*Writing passed:\s+(?<writingPassed>\d+)\s*Writing failed:\s+(?<writingFailed>\d+)\s*Total:\s+(?<total>\d+)'
    $match = [regex]::Match($stdout, $pattern, [System.Text.RegularExpressions.RegexOptions]::Singleline)
    if (-not $match.Success) {
        $result.Status = 'failed'
        $result.Error = 'Could not parse program output.'
        return [pscustomobject]$result
    }

    $result.PassedCount = [int]$match.Groups['passed'].Value
    $result.FailedCount = [int]$match.Groups['failed'].Value
    $result.Reading_us = [int64]$match.Groups['reading'].Value
    $result.Sorting_us = [int64]$match.Groups['sorting'].Value
    $result.Splitting_us = [int64]$match.Groups['splitting'].Value
    $result.WritingPassed_us = [int64]$match.Groups['writingPassed'].Value
    $result.WritingFailed_us = [int64]$match.Groups['writingFailed'].Value
    $result.Total_us = [int64]$match.Groups['total'].Value

    return [pscustomobject]$result
}

foreach ($fileCount in $files) {
    Write-Host "[$VariantName] Running $fileCount records..."
    try {
        $row = Invoke-BenchmarkRun -Executable $ExePath -BenchmarkRoot $InputRoot -RecordCount $fileCount
        $results += $row
        Write-Host "[$VariantName] Done $fileCount records: status=$($row.Status) peakMB=$($row.PeakWorkingSetMB) total_us=$($row.Total_us)"
    } catch {
        $results += [pscustomobject]@{
            Variant = $VariantName
            Container = $Container
            Strategy = $Strategy
            Size = $fileCount
            InputFile = Join-Path $InputRoot ("generated\students_{0}.txt" -f $fileCount)
            PassedCount = ''
            FailedCount = ''
            Reading_us = ''
            Sorting_us = ''
            Splitting_us = ''
            WritingPassed_us = ''
            WritingFailed_us = ''
            Total_us = ''
            PeakWorkingSetMB = ''
            Status = 'failed'
            Error = $_.Exception.Message
        }
        Write-Host "[$VariantName] FAILED $fileCount records: $($_.Exception.Message)"
    }
}

$results | Export-Csv -NoTypeInformation -Path $OutputCsv
Write-Host "[$VariantName] Wrote $OutputCsv"
$results | Format-Table -AutoSize | Out-String -Width 240 | Write-Host
