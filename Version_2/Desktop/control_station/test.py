import sys                                                                  

from PySide2.QtWidgets import QApplication, QPushButton                     

from PySide2.QtCore import QObject, Signal, Slot                            

                                                                            

app = QApplication(sys.argv)                                                

                                                                            

# define a new slot that receives a C 'int' or a 'str'                      

# and has 'saySomething' as its name                                        

@Slot(int)                                                                  

@Slot(str)                                                                  

def say_something(stuff):                                                   

    print(stuff)                                                            

                                                                            

class Communicate(QObject):                                                 

    # create two new signals on the fly: one will handle                    

    # int type, the other will handle strings                               

    speak_number = Signal(int)                                              

    speak_word = Signal(str)                                                  

                                                                            

someone = Communicate()                                                     

# connect signal and slot properly                                          

someone.speak_number.connect(say_something)                                 

someone.speak_word.connect(say_something)                                   

# emit each 'speak' signal                                                  

someone.speak_number.emit(10)                                               

someone.speak_word.emit("Hello everybody!")