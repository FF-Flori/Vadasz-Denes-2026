import tkinter as tk
import os
from tkinter import *
import tkinter.font

class Loglist:
    def __init__(self, main):
        self.main = main
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

        self.refbtn = tk.Button(self.header, text="\U0001F504 Újratöltés", font=tkinter.font.Font(family="Couriel New", size=15), cursor="hand2")
        self.refbtn.grid(row=0, rowspan=2, column=1, padx=50)

        self.yscrollbar = tk.Scrollbar(self.frame, orient=tk.VERTICAL)
        self.yscrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.frame2 = tk.Frame(self.frame)
        self.frame2.pack(fill=tk.BOTH, expand=True)
        
        self.listbox = tk.Listbox(self.frame2, yscrollcommand=self.yscrollbar)
        self.listbox.pack(fill=tk.BOTH, expand=True)

        self.erroriconlb = tk.Label(self.frame2, text="\U000024D8", font=tkinter.font.Font(family="Couriel New", size=18, weight="bold"))
        self.errorlb = tk.Label(self.frame2, text = "Sajnos nem található log.", font=tkinter.font.Font(family="Couriel New", size=18, weight="bold"))


        if len(os.listdir("log")) <= 0:
            self.listbox.pack_forget()
            self.frame2.config(highlightbackground="black", highlightthickness=1)
            self.frame2.pack(fill=tk.NONE, anchor=tk.CENTER)
            self.erroriconlb.grid(row=0, column=0)
            self.errorlb.grid(row=0, column=1)
            self.yscrollbar.pack_forget()
        else:
            for file in os.listdir("log"):
                self.listbox.insert(END, file)

        self.yscrollbar.config(command=self.listbox.yview)

main = tk.Tk()


window = Loglist(main)
main.mainloop()