param (
    [string]$assetPath = "assets/",
    [string]$packageName = "bin/main.zip",
    [string]$buildPath = "bin/assets/",
    [switch]$build = $false,
    [switch]$package = $false,
    [switch]$backup = $false,
    [switch]$silent = $false,
    [switch]$restore = $false,
    [switch]$clear = $false,
    [switch]$skipFonts = $false,
    [string]$backupPath = "G:/",
    [string]$toolsPath = "tools/"
)

$imageFileFilter = "*.png", "*.jpg", "*.jfif", "*.jpeg"
$shaderFileFilter = "*.glsl", "*.glsli"
$scriptFileFilter = "*.c", "*.h", "*.a"
$modelFileFilter = "*.json"
$meshFileFilter = "*.glb"
$fontFileFilter = "*.ttf"

$textureTool = "texconv"
$fontTool = "glfont"

$textureArgs = "-nologo -y"

$textureFormat = "BC3_UNORM"
$imageFormat = "R8G8B8A8_UNORM"
$fontTextureFormat = "BC4_UNORM"
$fontMapSize = "512"

if($clear) {
    if(!$silent) { Write-Host "Removing $buildPath" }
    if(Test-Path -Path $buildPath) {
        Remove-Item -Path $buildPath -Recurse
    }
}

if($build) {
    if(!$silent) { Write-Host "Building assets from $assetPath" }

    if(!$silent) { Write-Host "Rootfiles" -NoNewline }
    $subDir = ""
    $fileFilter = $imageFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Start-Process -FilePath "$toolsPath$textureTool" -Wait -NoNewWindow -ArgumentList $textureArgs, "-f $imageFormat", $file.Name -WorkingDirectory $workingDir -RedirectStandardOutput ".\NUL"
        Move-Item -Path "$workingDir$($file.BaseName).dds" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$silent) { Write-Host "Textures" -NoNewline }
    $subDir = "textures/"
    $fileFilter = $imageFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Start-Process -FilePath "$toolsPath$textureTool" -Wait -NoNewWindow -ArgumentList $textureArgs, "-f $textureFormat", $file.Name -WorkingDirectory $workingDir -RedirectStandardOutput ".\NUL"
        Move-Item -Path "$workingDir$($file.BaseName).dds" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$silent) { Write-Host "Shaders" -NoNewline }
    $subDir = "shaders/"
    $fileFilter = $shaderFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Copy-Item -Path "$workingDir$($file.Name)" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$silent) { Write-Host "Scripts" -NoNewline }
    $subDir = "scripts/"
    $fileFilter = $scriptFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Copy-Item -Path "$workingDir$($file.Name)" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$silent) { Write-Host "Models" -NoNewline }
    $subDir = "models/"
    $fileFilter = $modelFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Copy-Item -Path "$workingDir$($file.Name)" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$silent) { Write-Host "Meshes" -NoNewline }
    $subDir = "meshes/"
    $fileFilter = $meshFileFilter
    $workingDir = "$assetPath$subDir"
    $targetDir = "$buildPath$subDir"
    if(!(Test-Path -Path $targetDir)) {
        New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
    }
    $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
    foreach($file in $files) {
        if(!$silent) { Write-Host "." -NoNewline }
        Copy-Item -Path "$workingDir$($file.Name)" -Destination $targetDir -Force
    }
    if(!$silent) { Write-Host }

    if(!$skipFonts) {
        if(!$silent) { Write-Host "Fonts" -NoNewline }
        $subDir = "fonts/"
        $fileFilter = $fontFileFilter
        $workingDir = "$assetPath$subDir"
        $targetDir = "$buildPath$subDir"
        if(!(Test-Path -Path $targetDir)) {
            New-Item -Path $targetDir -ItemType Directory -Force | Out-Null
        }
        $files = Get-ChildItem -File -Path "$workingDir*" -Include $fileFilter
        foreach($file in $files) {
            if(!$silent) { Write-Host "." -NoNewline }
            Start-Process -FilePath "$toolsPath$fontTool" -Wait -NoNewWindow -ArgumentList $file.Name, $fontMapSize -WorkingDirectory $workingDir -RedirectStandardOutput ".\NUL"
            Start-Process -FilePath "$toolsPath$textureTool" -Wait -NoNewWindow -ArgumentList $textureArgs, "-f $fontTextureFormat", "$($file.BaseName).png" -WorkingDirectory $workingDir -RedirectStandardOutput ".\NUL"
            Move-Item -Path "$workingDir$($file.BaseName).json" -Destination $targetDir -Force
            Move-Item -Path "$workingDir$($file.BaseName).dds" -Destination $targetDir -Force
        }
        if(!$silent) { Write-Host }
    }
}

if($package) {
    if(!$silent) { Write-Host "Packaging $packageName" }
    if(Test-Path -Path $buildPath) {
        Compress-Archive -Path "$buildPath*" -DestinationPath $packageName -Force | Out-Null
    }
    else {
        Write-Host "Nothing to package. Try to build first"
    }
}

if($backup) {
    if(!$silent) { Write-Host "Backing up $assetPath" }
    $currentDate = Get-Date
    $backupDir = "$backupPath$($currentDate.ToString('yyyy-MM-dd-HH-mm-ss'))"
    New-Item -ItemType Directory -Path $backupDir | Out-Null
    Copy-Item -Path $assetPath -Destination $backupDir -Recurse -Force | Out-Null
}

if($restore) {

}

if(!$silent) { Write-Host "All Done!" }
