# File: main.py
import sys
import os
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QFile, QIODevice

import msv2

window = []
m = msv2.msv2()

def connect():
    device = window.connect_device.text()
    window.connect_status.clear()
    if(m.is_connected()):
        if(m.disconnect()):
            window.connect_status.insert("")
            window.connect_btn.setText("Connect")
        else:
            window.connect_status.insert("Error")
    else:

        if(m.connect(device)):
            window.connect_status.insert("Connected")
            window.connect_btn.setText("Disconect")
        else:
            window.connect_status.insert("Error")




if __name__ == "__main__":
    app = QApplication(sys.argv)

    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    ui_file_name = "mainwindow.ui"
    ui_file = QFile(ui_file_name)
    if not ui_file.open(QIODevice.ReadOnly):
        print("Cannot open {}: {}".format(ui_file_name, ui_file.errorString()))
        sys.exit(-1)
    loader = QUiLoader()
    window = loader.load(ui_file)
    ui_file.close()
    if not window:
        print(loader.errorString())
        sys.exit(-1)


    #connect all the callbacks
    window.connect_btn.clicked.connect(connect)


    window.show()

    sys.exit(app.exec_())
