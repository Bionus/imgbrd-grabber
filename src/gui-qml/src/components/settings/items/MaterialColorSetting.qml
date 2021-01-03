import QtQuick.Controls.Material 2.12

import ".."

ColorSetting {
    colors: globals.materialColors.map(c => Material.color(c, Material.Shade700))
    values: globals.materialColors
    labels: globals.materialColorsStr
    showLabels: false
}
