#!/usr/bin/env pwsh

# BS_thread_pool_test.ps1
# By Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
# v4.0.1, 2023-12-28
# Copyright (c) 2023 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
#
# BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This script compiles and runs the bundled test program with different compilers.

Set-StrictMode -Version Latest

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
    Set-Location $StartingLocation
    Exit $Code
}

Write-Title '=== BS::thread_pool test script ==='

$StartingLocation = Get-Location

Set-Location $PSScriptRoot

$SourceFile = Join-Path $PSScriptRoot 'BS_thread_pool_test.cpp'
If (Test-Path -Path $SourceFile)
{
    Write-Text "Found source file $SourceFile."
}
Else
{
    Write-Error "Source file $SourceFile not found, aborting script!"
    Exit-Script 1
}

$IncludeFile = Join-Path (Split-Path -Parent $PSScriptRoot) 'include' 'BS_thread_pool.hpp'
If (Test-Path -Path $IncludeFile)
{
    Write-Text "Found main include file $IncludeFile."
}
Else
{
    Write-Error "Main include file $IncludeFile not found, aborting script!"
    Exit-Script 1
}

$IncludeFile = Join-Path (Split-Path -Parent $PSScriptRoot) 'include' 'BS_thread_pool_utils.hpp'
If (Test-Path -Path $IncludeFile)
{
    Write-Text "Found utilities include file $IncludeFile."
}
Else
{
    Write-Error "Utilities include file $IncludeFile not found, aborting script!"
    Exit-Script 1
}

$BuildFolder = Join-Path (Split-Path -Parent $PSScriptRoot) 'build'
If (Test-Path -Path $BuildFolder)
{
    Write-Text "Cleaning up build folder $BuildFolder..."
    Remove-Item (Join-Path $BuildFolder '*')
}
Else
{
    Write-Text "Creating build folder $BuildFolder..."
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

$ExePrefix = 'BS_thread_pool_test_'

$Executables = @()

Function Build-ClangGCC([String] $Compiler, [String] $ExeSuffix, [String] $ExtraFlags = '')
{
    $FullExe = Join-Path $BuildFolder "$ExePrefix$ExeSuffix$Extension"
    $Command = "$Compiler $SourceFile$PThread -I../include -std=c++17 -O3 -march=native -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow -o $FullExe $ExtraFlags"
    Write-Command $Command
    Invoke-Expression $Command
    If ($LASTEXITCODE)
    {
        Write-Error "Failed to compile, aborting script! (Exit code: $LASTEXITCODE)"
        Exit-Script $LASTEXITCODE
    }
    Else
    {
        Write-Text "Successfully compiled to $FullExe."
        $Script:Executables += $FullExe
    }
}

If (Get-Command 'clang++' -ErrorAction SilentlyContinue)
{
    Write-Text 'Compiling with Clang...'
    Build-ClangGCC 'clang++' 'clang'
    Build-ClangGCC 'clang++' 'clang_light' '-DBS_THREAD_POOL_LIGHT_TEST'
}
Else
{
    Write-Error 'Clang not found, skipping it!'
}

Write-Host

If (Get-Command 'g++' -ErrorAction SilentlyContinue)
{
    Write-Text 'Compiling with GCC...'
    Build-ClangGCC 'g++' 'gcc' '-Wuseless-cast'
    Build-ClangGCC 'g++' 'gcc_light' '-Wuseless-cast -DBS_THREAD_POOL_LIGHT_TEST'
}
Else
{
    Write-Error 'GCC not found, skipping it!'
}

Write-Host

Function Build-MSVC([String] $ExeSuffix, [String] $ExtraFlags = '')
{
    $MSVCName = "$ExePrefix$ExeSuffix"
    $FullExe = Join-Path $BuildFolder "$MSVCName$Extension"
    $FullObj = Join-Path $BuildFolder "$MSVCName.obj"
    $Env:BS_THREAD_POOL_MSVC_COMMAND = "cl $SourceFile /I../include /std:c++17 /permissive- /O2 /W4 /EHsc /Fe:$FullExe /Fo:$FullObj $ExtraFlags"
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
        Remove-Item $FullObj
        Write-Text "Successfully compiled to $FullExe."
        $Script:Executables += $FullExe
    }
}

If ($IsWindows)
{
    $MSVCEnv = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1'
    If (Test-Path $MSVCEnv)
    {
        Write-Text 'Compiling with MSVC...'
        Build-MSVC 'msvc'
        Build-MSVC 'msvc_light' '/DBS_THREAD_POOL_LIGHT_TEST'
    }
    Else
    {
        Write-Error 'MSVC not found, skipping it!'
    }
}

Set-Location $BuildFolder

Write-Title 'Starting tests. Outputs can be found in the generated log files.'

ForEach ($Executable in $Executables)
{
    Write-Text "Executing: $Executable"
    Invoke-Expression $Executable | Out-Null
    If ($LASTEXITCODE)
    {
        Write-Error "Test Failed, aborting script! (Exit code: $LASTEXITCODE)"
        Exit-Script $LASTEXITCODE
    }
    Write-Text '-> Test finished successfully!'
    Write-Host
}

Write-Title '=== All tests completed successfully! ==='

Exit-Script 0
