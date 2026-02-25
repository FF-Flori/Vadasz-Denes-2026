import tkinter as tk
import os
from tkinter import *
import tkinter.font

def clearwidget(main):
    for widget in main.winfo_children():
        widget.destroy()

class Logexplorer:
    def __init__(self, main, logfile:str):
        self.main = main
        self.logfile = logfile
        self.main.title("Dashboard - Log kiválasztása")
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

        self.donebtn = tk.Button(self.frame2, text="Kész", font=tkinter.font.Font(family="Couriel New", size=15), cursor="hand2", command=lambda: self.done(logfile + "/" + str(self.listbox.selection_get())))
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
            clearwidget(self.main)
            Logexplorer(self.main, self.logfile)
    
    def done(self, logfile):
        if logfile != None and len(logfile) > 0:
            global window
            window = DashboardUI(self.main, logfile)
        else:
            print("Kötelező kijelölni 1 elemet!")
        
logline:int = None
time:list = []
battery:list = []
positionY:list = []
positionX:list = []
speed:list = []
distance:list = []
material:list = []

class backend:
    def __init__(self, logfile):
        if logfile != None and len(logfile) > 0:
            global logline
            global time
            global battery
            global positionY
            global positionX
            global speed
            global distance
            global material

            self.logfile = logfile
            self.refleshtime:int = 5

            with open(logfile, "r") as fullog:
                lines:int = 0
                for i in fullog.readlines():
                    if logline == None or lines > logline:
                        fullline:list = i.split(";")

                        positionX.append(fullline[0].split(",")[0])
                        positionY.append(fullline[0].split(",")[1])
                        battery.append(fullline[1])
                        speed.append(fullline[2])
                        distance.append(fullline[3])
                        material.append(fullline[4].replace("\n", ""))
                        
                        if len(time) == 0:
                            time.append(0)
                        else:
                            time.append(time[len(time)-1] + 30)
                    lines += 1

                fullog.close()
                logline = lines
        else:
            print("Kötelező kijelölni 1 elemet!")

class DashboardUI:
    def __init__(self, main: Tk, selectedlogfile):
        self.main = main
        self.selectedlogfile = selectedlogfile

        clearwidget(self.main)

        self.main.title("Dashboard")
        self.main.geometry("1200x800")
        self.main.resizable(False, False)

        BG = "#2e3237"
        BORDER = "#848690"

        
        self.frame = tk.Frame(self.main, bg=BG)
        self.frame.pack(fill="both", expand=True)

        
        self.header = tk.Frame(self.frame, bg=BG)
        self.header.pack(fill="x", padx=10, pady=10)

        self.refbtn = tk.Button(self.header, text="🔄 Újratöltés", font=tkinter.font.Font(family="Courier New", size=16), cursor="hand2", bg="#5c5c5c", fg="white", relief="flat", padx=10, pady=5)
        
        self.refbtn.pack(side="right")

        self.refbtn.bind("<Enter>", lambda e: self.refbtn.config(bg="#363535"))
        self.refbtn.bind("<Leave>", lambda e: self.refbtn.config(bg="#5c5c5c"))

        self.title = tk.Label(self.header, text="Dashboard", font=tkinter.font.Font(family="Courier New", size=30), fg="white", bg=BG)
        self.title.pack(anchor="w")

        self.subtitle = tk.Label(self.header, text="Logok betöltése 5 másodpercenként.", font=tkinter.font.Font(family="Courier New", size=16), fg="white", bg=BG)
        self.subtitle.pack(anchor="w")

        
        self.bodyframe = tk.Frame(self.frame, bg=BG)
        self.bodyframe.pack(fill="both", expand=True, padx=10, pady=10)

        
        for i in range(3):
            self.bodyframe.columnconfigure(i, weight=1)

        for i in range(2):
            self.bodyframe.rowconfigure(i, weight=1)

        
        def create_panel(parent, title, row, col, colspan=1):
            outer = tk.Frame(
                parent,
                bg=BG,
                highlightbackground=BORDER,
                highlightthickness=2
            )
            outer.grid(
                row=row,
                column=col,
                columnspan=colspan,
                sticky="nsew",
                padx=5,
                pady=5
            )

            outer.rowconfigure(1, weight=1)
            outer.columnconfigure(0, weight=1)

            title_label = tk.Label(
                outer,
                text=title,
                font=tkinter.font.Font(family="Courier New", size=14),
                fg="white",
                bg=BG
            )
            title_label.grid(row=0, column=0, sticky="ew")

            
            graph_area = tk.Frame(
                outer,
                bg="white"
            )
            graph_area.grid(
                row=1,
                column=0,
                sticky="nsew",
                padx=5,
                pady=5
            )

            return graph_area

        
        self.batteryframe = create_panel(
            self.bodyframe,
            "Akkumulátor Töltöttség (%)",
            0, 0
        )

        self.speedframe = create_panel(
            self.bodyframe,
            "Sebesség (km/h)",
            0, 1
        )

        self.distanceframe = create_panel(
            self.bodyframe,
            "Megtett Távolság",
            0, 2
        )

        
        self.materialframe = create_panel(
            self.bodyframe,
            "Gyűjtött Ásványok",
            1, 0
        )

        self.positionframe = create_panel(
            self.bodyframe,
            "Rover Pozíciója",
            1, 1,
            colspan=2
        )
        


main = tk.Tk()


window = Logexplorer(main, "log")

main.mainloop()