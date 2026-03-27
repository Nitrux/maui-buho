import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.maui.buho as Buho

import "widgets"
import "views/notes"

Maui.ApplicationWindow
{
    id: root
    title: notesView.currentTitle.length > 0 ? notesView.currentTitle + " — " + i18n("Buho") : i18n("Buho")

    color: "transparent"
    background: null

    Maui.WindowBlur
    {
        view: root
        geometry: Qt.rect(0, 0, root.width, root.height)
        windowRadius: Maui.Style.radiusV
        enabled: true
    }

    Rectangle
    {
        anchors.fill: parent
        color: Maui.Theme.backgroundColor
        opacity: 0.76
        radius: Maui.Style.radiusV
        border.color: Qt.rgba(1, 1, 1, 0)
        border.width: 1
    }

    readonly property font defaultFont : Maui.Style.defaultFont

    /***** COMPONENTS *****/
    Settings
    {
        id: settings
        category: "General"
        property bool autoSave: true
        property bool autoReload: true

        property font font : defaultFont
    }

    SettingsDialog
    {
        id: _settingsDialog
    }

    NotesView
    {
        id: notesView
        anchors.fill: parent
    }

    function newNote(content : string)
    {
        notesView.newNote(content)
    }
}
