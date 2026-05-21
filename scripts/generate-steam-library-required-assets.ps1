param(
    [string]$Source = "src/typescript/react/public/splash.png",
    [string]$OutputDir = "artifacts/steam-placeholders"
)

Add-Type -AssemblyName System.Drawing

$targets = @(
    @{ Name = "library_header_920x430.png"; W = 920; H = 430 },
    @{ Name = "library_hero_3840x1240.png"; W = 3840; H = 1240 },
    @{ Name = "library_logo_1280x720.png"; W = 1280; H = 720 }
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

    $dst = [System.Drawing.Bitmap]::new($t.W, $t.H, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($dst)
    $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.DrawImage($srcImg, [System.Drawing.Rectangle]::new(0, 0, $t.W, $t.H), [System.Drawing.Rectangle]::new($cropX, $cropY, $cropW, $cropH), [System.Drawing.GraphicsUnit]::Pixel)

    $outPath = Join-Path $OutputDir $t.Name
    $dst.Save($outPath, [System.Drawing.Imaging.ImageFormat]::Png)

    $g.Dispose()
    $dst.Dispose()
    Write-Output ("Generated {0} {1}x{2}" -f $t.Name, $t.W, $t.H)
}

$srcImg.Dispose()
