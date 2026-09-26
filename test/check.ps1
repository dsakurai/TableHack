Add-Type -AssemblyName System.Windows.Forms

$do = [Windows.Forms.Clipboard]::GetDataObject()

Write-Host "Available clipboard formats:"
foreach ($f in $do.GetFormats()) {
    Write-Host "  - $f"
}

$fmt = "HTML Format"
if ($do.GetDataPresent($fmt)) {
    Write-Host "`nFormat '$fmt' is present."
    $obj = $do.GetData($fmt)
    if ($null -eq $obj) {
        Write-Host "  Data object is null."
        exit
    }
    $bytes = $obj -as [byte[]]
    if ($null -eq $bytes -or $bytes.Length -eq 0) {
        Write-Host "  Data is not a byte[] or is empty."
        $s = $obj -as [string]
        if ($null -ne $s) {
            Write-Host "  As string (full):"
            $s | Set-Content clipboard_html_from_qt.txt
            Write-Host "  Written to clipboard_html_from_qt.txt"
        } else {
            Write-Host "  Cannot cast to string either."
        }
    } else {
        $text = [System.Text.Encoding]::UTF8.GetString($bytes)
        $text | Set-Content clipboard_html_from_qt.txt
        Write-Host "  Written to clipboard_html_from_qt.txt (from bytes)."
    }
} else {
    Write-Host "`nFormat '$fmt' is NOT present."
}