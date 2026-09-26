$htmlTable = @"
<table>
  <tr><td>A1</td><td>B1</td></tr>
  <tr><td>A2</td><td>B2</td></tr>
</table>
"@

Set-Clipboard -Value $htmlTable -AsHtml