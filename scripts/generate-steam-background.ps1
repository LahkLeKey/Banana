# PowerShell script to generate Steam page background (1438x810) from splash.png
# Save as scripts/generate-steam-background.ps1 and run from repo root

param(
    [string]$Source = "src/typescript/react/public/splash.png",
    [string]$Output = "artifacts/steam-placeholders/page_background_1438x810.png"
)

Add-Type -AssemblyName System.Drawing
$tw = 1438
$th = 810
$srcImg = [System.Drawing.Bitmap]::new($Source)
$srcW = [double]$srcImg.Width
$srcH = [double]$srcImg.Height
$targetRatio = $tw / $th
$srcRatio = $srcW / $srcH
if ($srcRatio -gt $targetRatio) {
    $cropH = [int]$srcH
    $cropW = [int][Math]::Round($srcH * $targetRatio)
    $cropX = [int][Math]::Floor(($srcW - $cropW) / 2)
    $cropY = 0
} else {
    $cropW = [int]$srcW
    $cropH = [int][Math]::Round($srcW / $targetRatio)
    $cropX = 0
    $cropY = [int][Math]::Floor(($srcH - $cropH) / 2)
}
$dst = [System.Drawing.Bitmap]::new($tw, $th, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($dst)
$g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
$g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
$g.DrawImage($srcImg, [System.Drawing.Rectangle]::new(0,0,$tw,$th), [System.Drawing.Rectangle]::new($cropX,$cropY,$cropW,$cropH), [System.Drawing.GraphicsUnit]::Pixel)
$dst.Save($Output, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose()
$dst.Dispose()
$srcImg.Dispose()
Write-Output ("Generated $Output ($tw x $th)")
