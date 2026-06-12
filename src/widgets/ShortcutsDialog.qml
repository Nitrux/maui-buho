import QtQuick.Controls

import org.mauikit.controls as Maui

Maui.SettingsDialog
{
    id: control

    Maui.Controls.title: i18n("Shortcuts")

    Maui.SectionGroup
    {
        title: i18n("Notes")

        Maui.FlexSectionItem
        {
            label1.text: i18n("New Note")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "N" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Paste from Clipboard")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "V" }
            }
        }
    }

    Maui.SectionGroup
    {
        title: i18n("Editor")

        Maui.FlexSectionItem
        {
            label1.text: i18n("Bold")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "B" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Italic")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "I" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Underline")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "U" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Insert Link")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "L" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Insert Image")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "Shift" }
                Action { text: "I" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Ordered List")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "Shift" }
                Action { text: "7" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Unordered List")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "Shift" }
                Action { text: "8" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Clear Formatting")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "Shift" }
                Action { text: "0" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Toggle Preview")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "F9" }
            }
        }

        Maui.FlexSectionItem
        {
            label1.text: i18n("Find")

            Maui.ToolActions
            {
                checkable: false
                autoExclusive: false
                Action { text: "Ctrl" }
                Action { text: "F" }
            }
        }
    }
}
