import QtQuick

import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

Maui.SettingsDialog
{
    id: control

    Component
    {
        id:_fontPageComponent

        Maui.SettingsPage
        {
            title: i18n("Font")

            Maui.FontPicker
            {
                Layout.fillWidth: true

                mfont: settings.font

                onFontModified:
                {
                    settings.font = font
                }
            }
        }
    }

    Maui.SectionGroup
    {
        title: i18n("Editor")

        Maui.FlexSectionItem
        {
            label1.text:  i18n("Auto Save")
            label2.text: i18n("Auto saves your file every few seconds")
            Switch
            {
                checkable: true
                checked: settings.autoSave
                onToggled: settings.autoSave = !settings.autoSave
            }
        }

        Maui.FlexSectionItem
        {
            label1.text:  i18n("Auto Reload")
            label2.text: i18n("Auto reload the text on external changes.")
            Switch
            {
                checkable: true
                checked: settings.autoReload
                onToggled: settings.autoReload = !settings.autoReload
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Font")
            label2.text: i18n("Font family and size.")

            ToolButton
            {
                checkable: true
                icon.name: "go-next"
                onToggled: control.addPage(_fontPageComponent)
            }
        }
    }
}
