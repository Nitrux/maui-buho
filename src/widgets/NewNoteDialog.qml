import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB
import org.mauikit.texteditor as TE

Maui.Page
{
    id: control

    property alias editor: _editor
    property alias document: _editor.document

    property string tagColor: note.color ? note.color : "transparent"
    property var note : ({})
    property int noteIndex : -1
    title: text.split("\n")[0]
    showTitle: false

    property alias text: _editor.text

    signal noteSaved(var note, int noteIndex)
    headerMargins: Maui.Style.defaultPadding

    headBar.visible: !editor.body.readOnly
    headBar.leftContent: [

        ToolSeparator
        {
            bottomPadding: 10
            topPadding: 10
        },

        ToolButton
        {
            icon.name: "format-text-bold"
            onClicked: control.wrapSelection("**", "**")
        },

        ToolButton
        {
            icon.name: "format-text-italic"
            onClicked: control.wrapSelection("*", "*")
        },

        ToolButton
        {
            icon.name: "format-text-underline"
            onClicked: control.wrapSelection("<u>", "</u>")
        },

        ComboBox
        {
            model: [i18n("Body"), i18n("Heading 1"), i18n("Heading 2"), i18n("Heading 3"), i18n("Heading 4")]
            onActivated: (index) => control.setHeading(index)
        },

        ToolSeparator
        {
            bottomPadding: 10
            topPadding: 10
        },

        ToolButton
        {
            icon.name: "insert-link"
            onClicked:
            {
                _linkText.text = _editor.body.selectedText
                _linkUrl.text = ""
                _linkDialog.open()
            }
        },

        ToolButton
        {
            icon.name: "insert-image"
            onClicked: _imagePicker.open()
        },

        ToolSeparator
        {
            bottomPadding: 10
            topPadding: 10
        },

        ToolButton
        {
            icon.name: "format-list-ordered"
            onClicked: control.prefixLine("1. ")
        },

        ToolButton
        {
            icon.name: "format-list-unordered"
            onClicked: control.prefixLine("- ")
        },

        ToolSeparator
        {
            bottomPadding: 10
            topPadding: 10
        },

        ToolButton
        {
            icon.name: "edit-clear"
            onClicked: control.clearFormat()
        }
    ]

    headBar.rightContent: [

        ToolButton
        {
            id: _previewButton
            icon.name: control.height > control.width ? "view-split-top-bottom" : "view-split-left-right"
            checkable: true
            checked: true
        },

        ToolSeparator
        {
            bottomPadding: 10
            topPadding: 10
        },

        ToolButton
        {
            icon.name: "edit-find"
            checkable: true
            checked: editor.showFindBar
            onClicked: editor.showFindBar = !editor.showFindBar
        }
    ]

    Dialog
    {
        id: _linkDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        title: i18n("Insert Link")
        width: Math.min(400, control.width * 0.8)
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout
        {
            width: parent.width
            spacing: Maui.Style.space.medium

            TextField
            {
                id: _linkText
                Layout.fillWidth: true
                placeholderText: i18n("Link text")
            }

            TextField
            {
                id: _linkUrl
                Layout.fillWidth: true
                placeholderText: i18n("https://...")
                inputMethodHints: Qt.ImhUrlCharactersOnly
            }
        }

        onAccepted:
        {
            var linkText = _linkText.text.length > 0 ? _linkText.text : _linkUrl.text
            var markdown = "[" + linkText + "](" + _linkUrl.text + ")"
            var body = _editor.body
            if (body.selectedText.length > 0)
                body.remove(body.selectionStart, body.selectionEnd)
            body.insert(body.cursorPosition, markdown)
        }
    }

    FB.FileDialog
    {
        id: _imagePicker
        singleSelection: true
        onUrlsSelected: (urls) =>
        {
            if (urls.length > 0)
            {
                var path = urls[0].toString()
                var altText = path.split("/").pop()
                _editor.body.insert(_editor.body.cursorPosition, "![" + altText + "](" + path + ")")
            }
        }
    }

    SplitView
    {
        anchors.fill: parent
        orientation: control.height > control.width ? Qt.Vertical : Qt.Horizontal

        TE.TextEditor
        {
            id: _editor
            SplitView.fillWidth: control.height <= control.width
            SplitView.fillHeight: control.height > control.width
            SplitView.minimumWidth: control.height <= control.width ? 200 : 0
            SplitView.minimumHeight: control.height > control.width ? 200 : 0

            fileUrl: control.note.url
            showLineNumbers: false
            document.autoReload: settings.autoReload
            document.autoSave: settings.autoSave
            body.font: settings.font
            document.enableSyntaxHighlighting: true
            document.formatName: "Markdown"
            body.placeholderText: i18n("Title\nBody")

            Rectangle
            {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                color: control.tagColor
                height: 12
            }

            Timer
            {
                id: _notifyTimer
                running: false
                interval: 2500
            }

            Maui.Chip
            {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: Maui.Style.space.big
                visible: _notifyTimer.running
                label.text: i18n("Note saved")
                iconSource: "document-save"
                Maui.Theme.backgroundColor: "yellow"
            }

            Connections
            {
                target: control.document
                function onFileSaved()
                {
                    console.log("NOTE SAVED")
                    _notifyTimer.start()
                }
            }
        }

        ScrollView
        {
            id: _previewPane
            visible: _previewButton.checked
            SplitView.preferredWidth: control.height <= control.width ? parent.width / 2 : 0
            SplitView.preferredHeight: control.height > control.width ? parent.height / 2 : 0
            SplitView.minimumWidth: (control.height <= control.width && visible) ? 200 : 0
            SplitView.minimumHeight: (control.height > control.width && visible) ? 200 : 0
            clip: true

            TextEdit
            {
                width: _previewPane.width
                padding: Maui.Style.defaultPadding
                text: _editor.text
                textFormat: TextEdit.MarkdownText
                readOnly: true
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                color: Maui.Theme.textColor
                selectionColor: Maui.Theme.highlightColor
                selectedTextColor: Maui.Theme.highlightedTextColor
                font: settings.font
            }
        }
    }

    // Wraps the selected text with before/after markers.
    // If nothing is selected, inserts markers with cursor placed between them.
    function wrapSelection(before, after)
    {
        var body = _editor.body
        if (body.selectedText.length > 0)
        {
            var selected = body.selectedText
            var start = body.selectionStart
            body.remove(body.selectionStart, body.selectionEnd)
            body.insert(start, before + selected + after)
        }
        else
        {
            var pos = body.cursorPosition
            body.insert(pos, before + after)
            body.cursorPosition = pos + before.length
        }
    }

    // Toggles a line prefix (e.g. "- " or "1. ") on the current line.
    function prefixLine(prefix)
    {
        var body = _editor.body
        var pos = body.cursorPosition
        var lineStart = body.text.lastIndexOf('\n', pos - 1) + 1

        if (body.text.substring(lineStart, lineStart + prefix.length) === prefix)
            body.remove(lineStart, lineStart + prefix.length)
        else
            body.insert(lineStart, prefix)
    }

    // Sets the heading level (1–4) on the current line, or removes it when level is 0.
    function setHeading(level)
    {
        var body = _editor.body
        var text = body.text
        var pos = body.cursorPosition
        var lineStart = text.lastIndexOf('\n', pos - 1) + 1
        var lineEnd = text.indexOf('\n', pos)
        if (lineEnd === -1) lineEnd = text.length

        var stripped = text.substring(lineStart, lineEnd).replace(/^#+\s*/, "")
        var newLine = level > 0 ? "#".repeat(level) + " " + stripped : stripped

        body.remove(lineStart, lineEnd)
        body.insert(lineStart, newLine)
    }

    // Removes inline Markdown markers from the selection, or clears line-level
    // formatting (headings, list prefixes) when nothing is selected.
    function clearFormat()
    {
        var body = _editor.body
        if (body.selectedText.length > 0)
        {
            var start = body.selectionStart
            var cleaned = body.selectedText
                .replace(/\*\*(.*?)\*\*/g, "$1")
                .replace(/\*(.*?)\*/g, "$1")
                .replace(/_(.*?)_/g, "$1")
                .replace(/~~(.*?)~~/g, "$1")
                .replace(/<u>(.*?)<\/u>/g, "$1")
            body.remove(body.selectionStart, body.selectionEnd)
            body.insert(start, cleaned)
        }
        else
        {
            setHeading(0)
        }
    }

    function update()
    {
        _editor.fileUrl = Qt.binding(() => { return control.note.url })
    }

    function clear()
    {
        _editor.body.clear()
        control.note = ({})
    }

    function packNote()
    {
        var note = ({})
        const content = _editor.body.text
        if (content.length > 0)
        {
            note = {
                url: _editor.fileUrl,
                content: content,
                format: ".md"
            }

            if (control.tagColor !== "transparent")
            {
                note["color"] = control.tagColor
            }
        }
        return note
    }

    function saveNote()
    {
        if (FB.FM.fileExists(document.fileUrl))
        {
            if (document.modified)
            {
                console.log("Saving a new note but existing file", document.fileUrl)
                document.saveAs(document.fileUrl)
            }

            console.log("trying to update note in the main model", noteIndex, notesModel.mappedToSource(noteIndex))
            list.update(packNote(), notesModel.mappedToSource(noteIndex))
            return packNote()
        }
        else
        {
            return list.insert(packNote())
        }
    }
}
