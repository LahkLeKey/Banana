param(
    [string]$Source = "src/typescript/react/public/splash.png",
    [string]$OutputDir = "artifacts/steam-placeholders/screenshots"
)

Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

$tw = 1920
$th = 1080

# Slightly different crop anchors to produce 5 distinct placeholder screenshots
$variants = @(
    @{Name="screenshot_01_1920x1080.png"; XBias=0.5; YBias=0.5},
    @{Name="screenshot_02_1920x1080.png"; XBias=0.3; YBias=0.5},
    @{Name="screenshot_03_1920x1080.png"; XBias=0.7; YBias=0.5},
    @{Name="screenshot_04_1920x1080.png"; XBias=0.5; YBias=0.35},
    @{Name="screenshot_05_1920x1080.png"; XBias=0.5; YBias=0.65}
)

$srcImg = [System.Drawing.Bitmap]::new($Source)
$srcW = [double]$srcImg.Width
$srcH = [double]$srcImg.Height
$targetRatio = [double]$tw / [double]$th
$srcRatio = $srcW / $srcH

foreach ($v in $variants) {
    if ($srcRatio -gt $targetRatio) {
        $cropH = [int]$srcH
        $cropW = [int][Math]::Round($srcH * $targetRatio)
        $maxX = [int]($srcW - $cropW)
        $cropX = [int][Math]::Round($maxX * [double]$v.XBias)
        $cropY = 0
    } else {
        $cropW = [int]$srcW
        $cropH = [int][Math]::Round($srcW / $targetRatio)
        $cropX = 0
        $maxY = [int]($srcH - $cropH)
        $cropY = [int][Math]::Round($maxY * [double]$v.YBias)
    }

    $dst = [System.Drawing.Bitmap]::new($tw, $th, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($dst)
    $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.DrawImage($srcImg, [System.Drawing.Rectangle]::new(0, 0, $tw, $th), [System.Drawing.Rectangle]::new($cropX, $cropY, $cropW, $cropH), [System.Drawing.GraphicsUnit]::Pixel)

    $outPath = Join-Path $OutputDir $v.Name
    $dst.Save($outPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $dst.Dispose()

    Write-Output ("Generated {0} {1}x{2}" -f $v.Name, $tw, $th)
}

$srcImg.Dispose()
