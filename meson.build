project('buho', 'cpp', version: '4.0.5', meson_version: '>=1.12.0', default_options: ['cpp_std=c++17', 'buildtype=release'])
qt6 = import('qt6')
cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.get_id() == 'gcc' and cpp_compiler.has_argument('-Wno-sfinae-incomplete')
    add_project_arguments('-Wno-sfinae-incomplete', language: 'cpp')
endif
qt = dependency('qt6', version: '>=6.9', modules: ['Core', 'DBus', 'Network', 'Qml', 'Quick', 'Sql'])
kf6_core = dependency('KF6CoreAddons', version: '>=6.13')
kf6_i18n = declare_dependency(
    include_directories: include_directories(get_option('prefix') / 'include' / 'KF6' / 'KI18n'),
    dependencies: [cpp_compiler.find_library('KF6I18n'), cpp_compiler.find_library('KF6I18nQml')],
)
mauikit = dependency('MauiKit4', version: '>=4.0.5')
filebrowsing = dependency('MauiKitFileBrowsing4', version: '>=4.0.5')
texteditor = dependency('MauiKitTextEditor4', version: '>=4.0.5')
cpp_args = ['-DAPP_COPYRIGHT_NOTICE="© 2019-2025 Maui Development Team"']
if host_machine.cpu_family() == 'x86_64'
    cpp_args += ['-march=x86-64-v3']
endif
buho_version = configure_file(
    input: 'meson-content.in',
    output: 'buho_version.h',
    configuration: {'CONTENT': ('''#pragma once
#define BUHO_VERSION_STRING "4.0.5"
#define GIT_BRANCH "Stable"
#define GIT_COMMIT_HASH "4.0.5"
''')}
)
buho_includes = include_directories('src', 'src/utils', 'src/syncing', 'src/providers', 'src/controllers', 'src/models')
dbus_cpp2xml = find_program('qdbuscpp2xml')
dbus_xml2cpp = find_program('qdbusxml2cpp')
dbus_xml = custom_target('buho-dbus-xml', input: 'src/utils/server.h', output: 'org.kde.Buho.xml', command: [dbus_cpp2xml, '-o', '@OUTPUT@', '@INPUT@'])
dbus_adaptor = custom_target('buho-dbus-adaptor', input: dbus_xml, output: ['buhoadaptor.cpp', 'buhoadaptor.h'], command: [dbus_xml2cpp, '-a', 'buhoadaptor', '@INPUT@'])
dbus_interface = custom_target('buho-dbus-interface', input: dbus_xml, output: ['buhointerface.cpp', 'buhointerface.h'], command: [dbus_xml2cpp, '-p', 'buhointerface', '@INPUT@'])
qt_sources = qt6.preprocess(qresources: ['src/assets.qrc', 'src/assets/imgs.qrc', 'src/qml.qrc'], moc_headers: ['src/utils/server.h', 'src/db/db.h', 'src/models/notes/notes.h', 'src/providers/abstractnotesprovider.h', 'src/providers/nextnote.h', 'src/syncing/syncer.h', 'src/syncing/notessyncer.h', 'src/controllers/notes/notescontroller.h', dbus_adaptor[1], dbus_interface[1]])
executable('buho', ['src/main.cpp', 'src/db/db.cpp', 'src/models/notes/notes.cpp', 'src/providers/nextnote.cpp', 'src/syncing/syncer.cpp', 'src/syncing/notessyncer.cpp', 'src/controllers/notes/notescontroller.cpp', 'src/utils/server.cpp', dbus_adaptor, dbus_interface, buho_version] + qt_sources, dependencies: [qt, kf6_core, kf6_i18n, mauikit, filebrowsing, texteditor], include_directories: buho_includes, cpp_args: cpp_args, install: true)
install_data('src/org.maui.buho.desktop', install_dir: get_option('datadir') / 'applications')
install_data('src/org.maui.buho.metainfo.xml', install_dir: get_option('datadir') / 'metainfo')
install_data('src/assets/buho.svg', install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps')

translation_domain = 'buho'
msgfmt = find_program('msgfmt')
po_files = run_command('find', 'po', '-mindepth', '2', '-maxdepth', '2', '-type', 'f', '-name', translation_domain + '.po', check: true).stdout().strip().split('\n')
foreach po_file : po_files
    if po_file != ''
        locale = po_file.split('/')[1]
        custom_target(
            'translation-' + locale,
            input: po_file,
            output: locale / (translation_domain + '.mo'),
            command: [msgfmt, '--check', '-o', '@OUTPUT@', '@INPUT@'],
            install: true,
            install_dir: get_option('localedir') / locale / 'LC_MESSAGES',
            install_tag: 'i18n')
    endif
endforeach
