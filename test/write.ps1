Add-Type -AssemblyName System.Windows.Forms

# 1. Set HTML table on clipboard
$htmlTable = @"
<table>
  <tr><td>A1</td><td>B1</td></tr>
  <tr><td>A2</td><td>B2</td></tr>
</table>
"@

Set-Clipboard -Value $htmlTable -AsHtml
Write-Host "HTML table copied to clipboard."

# 2. Inspect all formats
$do = [Windows.Forms.Clipboard]::GetDataObject()

Write-Host "Available clipboard formats:"
foreach ($f in $do.GetFormats()) {
    Write-Host "  - $f"
}

# 3. Try to read anything that might be HTML
$candidates = @(
    "HTML Format",
    "text/html",
    "Html",
    "HTML",
    "CF_HTML"
)

foreach ($fmt in $candidates) {
    if ($do.GetDataPresent($fmt)) {
        Write-Host "`nFormat '$fmt' is present."
        $obj = $do.GetData($fmt)
        if ($null -eq $obj) {
            Write-Host "  Data object is null."
            continue
        }
        $bytes = $obj -as [byte[]]
        if ($null -eq $bytes -or $bytes.Length -eq 0) {
            Write-Host "  Data is not a byte[] or is empty."
            # Try as string
            $s = $obj -as [string]
            if ($null -ne $s) {
                Write-Host "  As string (first 200 chars):"
                Write-Host $s.Substring(0, [Math]::Min(200, $s.Length))
            }
        } else {
            $text = [System.Text.Encoding]::UTF8.GetString($bytes)
            Write-Host "  First 300 chars as UTF-8 string:"
            Write-Host $text.Substring(0, [Math]::Min(300, $text.Length))
        }
    } else {
        Write-Host "`nFormat '$fmt' is NOT present."
    }
}