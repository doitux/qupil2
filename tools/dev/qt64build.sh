#!/bin/bash
# Local check with Qt 6.4 (Ubuntu packages; CI and the devices use Qt 6.8): copies the working tree to
# $QUPIL_WORK/qt64test, adapts the 6.5+ APIs to 6.4 and builds. Exits with the build's exit code.
S=${QUPIL_WORK:-/home/claude/work}
T=$S/qt64test
mkdir -p $T
set -e
rm -rf $T/src-tree && mkdir -p $T/src-tree
cd "$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
{ git ls-files -z; git ls-files -o --exclude-standard -z; } | xargs -0 -I{} cp --parents {} $T/src-tree/
cd $T/src-tree
[ "$(pwd)" = "$T/src-tree" ] || exit 1
python3 - <<'EOF'
from pathlib import Path
import re
c = Path('CMakeLists.txt'); s = c.read_text()
s = s.replace('find_package(Qt6 6.5 REQUIRED', 'find_package(Qt6 6.4 REQUIRED').replace('qt_standard_project_setup(REQUIRES 6.5)', 'qt_standard_project_setup()').replace('OUTPUT_SCRIPT qupil_deploy_script', 'FILENAME_VARIABLE qupil_deploy_script')
c.write_text(s)
m = Path('src/main.cpp'); s = m.read_text()
s = s.replace('engine.loadFromModule("Qupil", "Main");', 'engine.addImportPath(u"qrc:/"_s);\n    engine.load(QUrl(u"qrc:/Qupil/Main.qml"_s));')
s = re.sub(r'engine\.singletonInstance<QObject \*>\("Qupil", "(\w+)"\)', lambda m: '[&]{ QQmlComponent c(&engine); c.setData("import QtQml\\nimport Qupil\\nQtObject { property QtObject t: ' + m.group(1) + ' }", QUrl(u"qrc:/Qupil/x.qml"_s)); QObject *o = c.create(); return o ? o->property("t").value<QObject *>() : nullptr; }()', s)
s = re.sub(r'engine\.singletonInstance<(\w+) \*>\("Qupil", "(\w+)"\)', r'engine.singletonInstance<\1 *>(qmlTypeId("Qupil", 1, 0, "\2"))', s)
s = s.replace('#include <QQmlApplicationEngine>', '#include <QQmlApplicationEngine>\n#include <QQmlComponent>')
s = s.replace('u":/qt/qml/Qupil/assets/fonts"_s', 'u":/Qupil/assets/fonts"_s')
m.write_text(s)
for f in Path('src/app').glob('*.cpp'):
    x = f.read_text()
    y = x.replace('":/qt/qml/Qupil/', '":/Qupil/')
    if y != x: f.write_text(y)
t = Path('qml/Theme.qml'); s = t.read_text()
s = s.replace('import QtCore\n', '')
s = s.replace('readonly property bool systemDark: Qt.styleHints.colorScheme === Qt.ColorScheme.Dark', 'readonly property bool systemDark: false')
s = s.replace(': Qt.styleHints.colorScheme === Qt.ColorScheme.Dark', ': false')
s = re.sub(r'    property Settings _store: Settings \{.*?\n    \}\n', '', s, flags=re.S)
s = s.replace('onModeChanged: _applyNativeScheme()', '').replace('Component.onCompleted: _applyNativeScheme()', '')
t.write_text(s)
n = Path('qml/Nav.qml'); n.write_text(n.read_text().replace('import QtQuick\n', 'import QtQuick\nimport Qupil\n', 1))
for p in Path('qml').rglob('*.qml'):
    s = p.read_text()
    s2 = s.replace('import QtCore\n', 'import Qt.labs.settings\n') if 'Settings {' in s and p.name != 'Theme.qml' else s
    if s2 != s: p.write_text(s2)
EOF
set +e
if [ ! -f $T/build/build.ninja ]; then
  cmake -S $T/src-tree -B $T/build -G Ninja -DCMAKE_BUILD_TYPE=Debug > $T/cfg.log 2>&1 || { tail -30 $T/cfg.log; exit 1; }
fi
cmake --build $T/build 2>&1 | grep -E "error|warning:|FAILED|undefined" | head -40
rc=${PIPESTATUS[0]}; echo "build exit $rc"; exit $rc
