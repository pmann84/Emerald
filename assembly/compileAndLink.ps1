Param(
    [string]$inputFile
)
if (-Not ($PSBoundParameters.ContainsKey('inputFile'))) {
    Write-Error "No input assembly file specified..."
    exit 1
}

# Get the file name
$filename = (Get-Item $inputFile).BaseName
$baseDir = (Get-Item $inputFile).Directory.FullName

# Construct output file names
$outputObjFile = $baseDir + "\" + $filename + ".obj"
$outputExe = $baseDir + "\" + $filename + ".exe"

# Assembler
Write-Host "Generating object file from:" $inputFile "->" $outputObjFile
nasm -fwin64 $inputFile -o $outputObjFile # (--prefix \_)

if (-Not (Test-Path -Path $outputObjFile -PathType Leaf)) {
    Write-Error "No object file generated...check for errors in the assembler..."
    exit 1
}

# Link
Write-Host "Linking" $outputObjFile "->" $outputExe
# $linkerPath = "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.34.31933\bin\Hostx64\x64\link.exe"
link /MACHINE:X64 /SUBSYSTEM:CONSOLE /OUT:$outputExe /NODEFAULTLIB /ENTRY:start $outputObjFile # (requires VS prompt to get link on the path)
