# PowerShell script to generate all required Steam library assets from splash.png
# Save as scripts/generate-steam-library-assets.ps1 and run from repo root

param(
    [string]$Source = "src/typescript/react/public/splash.png",
    [string]$OutputDir = "artifacts/steam-placeholders"
)

Add-Type -AssemblyName System.Drawing
$targets = @(
    @{Name="library_600x900.png"; W=600; H=900},
    @{Name="library_300x450.png"; W=300; H=450},
    @{Name="library_logo_184x69.png"; W=184; H=69}
)
$srcImg = [System.Drawing.Bitmap]::new($Source)
foreach ($t in $targets) {
    $tw = [double]$t.W
    $th = [double]$t.H
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
    $outPath = Join-Path $OutputDir $t.Name
    $dst.Save($outPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $dst.Dispose()
    Write-Output ("Generated $($t.Name) $tw x $th")
}
$srcImg.Dispose()
