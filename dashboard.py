import tkinter as tk
import os
from tkinter import *
import tkinter.font

class Logexplorer:
    def __init__(self, main, logfile:str):
        self.main = main
        self.logfile = logfile
        self.main.title("Dashboard")
        self.main.geometry("400x400")
        self.main.resizable(False,False)

        self.frame = tk.Frame(self.main)
        self.frame.pack(fill=tk.BOTH, expand=True)

        self.header = tk.Frame(self.frame)
        self.header.pack(fill=tk.X)

        self.label = tk.Label(self.header, text= "Log kiválasztása", font= tkinter.font.Font(family="Couriel New", size=15))
        self.label.grid(row=0, column=0)

        self.sublabel = tk.Label(self.header, text="Kérjük válasszon egy dátumot!")
        self.sublabel.grid(row=1, column=0)

        self.refbtn = tk.Button(self.header, text="\U0001F504 Újratöltés", font=tkinter.font.Font(family="Couriel New", size=15), cursor="hand2", command=lambda: self.reflesh())
        self.refbtn.grid(row=0, rowspan=2, column=1, padx=50)

        self.yscrollbar = tk.Scrollbar(self.frame, orient=tk.VERTICAL)
        self.yscrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.frame2 = tk.Frame(self.frame)
        self.frame2.pack(fill=tk.BOTH, expand=True)
        
        self.listbox = tk.Listbox(self.frame2, yscrollcommand=self.yscrollbar.set)
        self.listbox.pack(fill=tk.BOTH, expand=True)
        self.yscrollbar.config(command=self.listbox.yview)

        self.errorframe = tk.Frame(self.frame2)
        self.erroriconlb = tk.Label(self.errorframe, text="\U000024D8", font=tkinter.font.Font(family="Couriel New", size=20, weight="bold"))
        self.errorlb = tk.Label(self.errorframe, text="Sajnos nem található log.", font=tkinter.font.Font(family="Couriel New", size=18, weight="bold"))
        self.erroriconlb.pack(side=tk.LEFT, padx=5)
        self.errorlb.pack(side=tk.LEFT, padx=5)

        self.donebtn = tk.Button(self.frame2, text="Kész", font=tkinter.font.Font(family="Couriel New", size=15), cursor="hand2", command=lambda: print(backend(logfile + "/" + str(self.listbox.selection_get())).gettime()))
        self.donebtn.pack(side=BOTTOM)

        if len(os.listdir(logfile)) <= 0:
            self.listbox.pack_forget()
            self.yscrollbar.pack_forget()
            self.donebtn.pack_forget()

            self.errorframe.pack(expand=True)

        else:
            self.errorframe.pack_forget()
            self.listbox.pack(fill=tk.BOTH, expand=True)
            self.yscrollbar.pack(side=tk.RIGHT, fill=tk.Y)
            self.donebtn.pack(side=tk.BOTTOM, fill=tk.X)

            self.listbox.delete(0, END)
            for file in os.listdir(logfile):
                self.listbox.insert(END, file)

            self.listbox.yview_moveto(0)

    def reflesh(self):

        if len(os.listdir(self.logfile)) <= 0:
            self.listbox.pack_forget()
            self.yscrollbar.pack_forget()
            self.donebtn.pack_forget()

            self.errorframe.pack(expand=True)

        else:
            for widget in self.main.winfo_children():
                widget.destroy()
                
            Logexplorer(self.main, self.logfile)
        
logline:int = None

class backend:
    def __init__(self, logfile):
        global logline

        self.logfile = logfile
        self.time:list = []
        self.refleshtime = 5
        self.battery:list = []

        with open(logfile, "r") as fullog:
            lines:int = 0
            for i in fullog.readlines():
                if logline == None or lines < logline:
                    fullline:list = i.split(";")
                    self.battery.append(fullline[1])
                    if len(self.time) == 0:
                        self.time.append(0)
                    else:
                        self.time.append(self.time[len(self.time)-1] + 30)
                lines += 1

            fullog.close()
            logline = lines

    def getbattery(self):
        return self.battery
    
    def gettime(self):
        return self.time


main = tk.Tk()


window = Logexplorer(main, "log")

main.mainloop()