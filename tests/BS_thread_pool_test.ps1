#!/usr/bin/env pwsh
$Host.UI.RawUI.WindowTitle = 'BS::thread_pool Test Script'

$TitleColor = 'Green'
$TextColor = 'Yellow'
$ErrorColor = 'Red'
$CommandColor = 'Blue'

Function Write-Title([String] $Title)
{
    If ($Title.Length -gt 0)
    {
        $Separator = '=' * $Title.Length
        Write-Host
        Write-Host $Separator -ForegroundColor $TitleColor
        Write-Host $Title -ForegroundColor $TitleColor
        Write-Host $Separator -ForegroundColor $TitleColor
        Write-Host
    }
}

Function Write-Text([String] $Text)
{
    Write-Host $Text -ForegroundColor $TextColor
}

Function Write-Error([String] $Text)
{
    Write-Host $Text -ForegroundColor $ErrorColor
}

Function Write-Command([String] $Text)
{
    Write-Host $Text -ForegroundColor $CommandColor
}

Function Exit-Script([Int] $Code)
{
    Set-Location $CurrentDir
    Exit $Code
}

$CurrentDir = Get-Location

Write-Title '=== BS::thread_pool test script ==='

$SourceFile = 'BS_thread_pool_test.cpp'
If (Test-Path -Path $SourceFile)
{
    Write-Text "Found source file $SourceFile."
}
Else
{
    Write-Error "Source file $SourceFile not found, aborting script!"
    Exit-Script 1
}

$BuildFolder = Join-Path '..' 'build'
If (Test-Path -Path $BuildFolder)
{
    Write-Text 'Cleaning up build folder...'
    Remove-Item (Join-Path $BuildFolder '*')
}
Else
{
    Write-Text 'Creating build folder...'
    $Null = New-Item $BuildFolder -ItemType 'Directory'
}
Write-Text 'Done.'

Write-Title 'Compiling...'

If ($IsWindows)
{
    $Extension = '.exe'
    $PThread = ''
}
Else
{
    $Extension = ''
    $PThread = ' -pthread'
}

$ClangExe = 'BS_thread_pool_test_clang' + $Extension
$GCCExe = 'BS_thread_pool_test_gcc' + $Extension

$MSVCName = 'BS_thread_pool_test_msvc'
$MSVCExe = $MSVCName + $Extension
$MSVCObj = $MSVCName + '.obj'

If (Get-Command 'clang++' -ErrorAction SilentlyContinue)
{
    $Clang = $True
    $FullClangExe = Join-Path $BuildFolder $ClangExe
    Write-Text 'Compiling with Clang...'
    $ClangCommand = "clang++ $SourceFile$PThread -std=c++17 -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow -o $FullClangExe"
    Write-Command $ClangCommand
    Invoke-Expression $ClangCommand
    If ($LASTEXITCODE)
    {
        Write-Error "Failed to compile, aborting script! (Exit code: $LASTEXITCODE)"
        Exit-Script $LASTEXITCODE
    }
    Else
    {
        Write-Text "Successfully compiled to $FullClangExe."
    }
}
Else
{
    $Clang = $False
    Write-Error 'Clang not found, skipping it!'
}

Write-Host

If (Get-Command 'g++' -ErrorAction SilentlyContinue)
{
    $GCC = $True
    $FullGCCExe = Join-Path $BuildFolder $GCCExe
    Write-Text 'Compiling with GCC...'
    $GCCCommand = "g++ $SourceFile$PThread -std=c++17 -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow -o $FullGCCExe"
    Write-Command $GCCCommand
    Invoke-Expression $GCCCommand
    If ($LASTEXITCODE)
    {
        Write-Error "Failed to compile, aborting script! (Exit code: $LASTEXITCODE)"
        Exit-Script $LASTEXITCODE
    }
    Else
    {
        Write-Text "Successfully compiled to $FullGCCExe."
    }
}
Else
{
    $GCC = $False
    Write-Error 'GCC not found, skipping it!'
}

Write-Host

If ($IsWindows)
{
    $MSVCEnv = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1'
    If (Test-Path $MSVCEnv)
    {
        $MSVC = $True
        $FullMSVCExe = Join-Path $BuildFolder $MSVCExe
        $FullMSVCObj = Join-Path $BuildFolder $MSVCObj
        Write-Text 'Compiling with MSVC...'
        $Env:BS_THREAD_POOL_MSVC_COMMAND = "cl $SourceFile /std:c++17 /permissive- /O2 /W4 /EHsc /Fe:$FullMSVCExe /Fo:$FullMSVCObj"
        Write-Command $Env:BS_THREAD_POOL_MSVC_COMMAND
        pwsh -Command {
            $CurrentDir = Get-Location
            & 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1'
            Set-Location $CurrentDir
            Invoke-Expression $Env:BS_THREAD_POOL_MSVC_COMMAND
            Exit $LASTEXITCODE
        }
        $Env:BS_THREAD_POOL_MSVC_COMMAND = $Null
        If ($LASTEXITCODE)
        {
            Write-Error "Failed to compile, aborting script! (Exit code: $LASTEXITCODE)"
            Exit-Script $LASTEXITCODE
        }
        Else
        {
            Remove-Item $FullMSVCObj
            Write-Text "Successfully compiled to $FullMSVCExe."
        }
    }
    Else
    {
        $MSVC = $False
        Write-Error 'MSVC not found, skipping it!'
    }
}
Else
{
    $MSVC = $False
}

Write-Title 'Starting tests...'

Set-Location $BuildFolder
$Repeats = 5;

Write-Text "Each test will be repeated $Repeats times. Outputs can be found in the generated log files."

Function Invoke-Test([String] $Compiler, [String] $Executable)
{
    Write-Title "Starting $Compiler tests..."
    For ($i = 1; $i -Le $Repeats; $i++)
    {
        Write-Text "Running $Compiler test #$i of $Repeats..."
        Invoke-Expression $Executable | Out-Null
        if ($LASTEXITCODE)
        {
            Write-Error "Test Failed, aborting script! (Exit code: $LASTEXITCODE)"
            Exit-Script $LASTEXITCODE
        }
        Write-Text 'Test finished successfully!'
        Write-Host
    }
    Write-Text "$Compiler tests done."
}

if ($Clang)
{
    Invoke-Test 'Clang' (Join-Path '.' $ClangExe)
}
if ($GCC)
{
    Invoke-Test 'GCC' (Join-Path '.' $GCCExe)
}
if ($MSVC)
{
    Invoke-Test 'MSVC' (Join-Path '.' $MSVCExe)
}

Write-Title '=== All tests completed successfully! ==='

Exit-Script 0
