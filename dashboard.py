import tkinter as tk
import os
from tkinter import *
import tkinter.font
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import numpy as np

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
        
logline:int = 0
logsize:int = 0
time:list = []
battery:list = []
positionY:list = []
positionX:list = []
speed:list = []
distance:list = []
materialB:int = 0
materialY:int = 0
materialG:int = 0

def backend(logfile):
    global logsize
    if logfile != None and len(logfile) > 0 and logsize < os.path.getsize(logfile):
        global logline
        global time
        global battery
        global positionY
        global positionX
        global speed
        global distance
        global materialB
        global materialY
        global materialG

        with open(logfile, "r") as fullog:
            lines = fullog.readlines()

            for i in range(logline, len(lines)):
                fullline:list = lines[i].split(";")

                positionX.append(int(fullline[0].split(",")[0]))
                positionY.append(int(fullline[0].split(",")[1]))
                battery.append(int(fullline[1]))
                speed.append(int(fullline[2]))
                distance.append(int(fullline[3]))
                materialB += int(fullline[4].split(",")[0].replace("\n", ""))
                materialY += int(fullline[4].split(",")[1].replace("\n", ""))
                materialG += int(fullline[4].split(",")[2].replace("\n", ""))
                        
                if len(time) == 0:
                    time.append(0)
                else:
                    time.append(time[len(time)-1] + 30)

            fullog.close()
            logline = len(lines)
            logsize = os.path.getsize(logfile)

class DashboardUI:
    def __init__(self, main: Tk, selectedlogfile):
        self.main = main
        self.selectedlogfile = selectedlogfile
        self.refleshing = False

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

        self.refbtn = tk.Button(self.header, text="🔄 Újratöltés", font=tkinter.font.Font(family="Courier New", size=16), cursor="hand2", bg="#5c5c5c", fg="white", relief="flat", padx=10, pady=5, command=lambda: self.refleshprg(False))
        
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
                bg=BG,
                highlightbackground="gray",
                highlightthickness=1
            )
            title_label.grid(row=0, column=0, sticky="ew")
            
            graph_area = tk.Frame(
                outer,
                bg=BG
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

        #Battery diagram
        self.batteryfg, self.batteryax=plt.subplots(figsize=(4,3), dpi=100)
        self.batteryax.plot([], [], marker="o", color="lightgreen")
        self.batteryax.set_ylim(0, 100)
        self.batteryax.set_ylabel("Töltöttség (%)")
        self.batteryax.set_xlabel("Idő (perc)")
        self.batteryax.grid(True, linestyle="--", linewidth=0.5)
        self.batterycanvas = FigureCanvasTkAgg(self.batteryfg, master=self.batteryframe)
        self.batterycanvas.get_tk_widget().pack(fill="both", expand=True)
        self.batteryfg.patch.set_facecolor(BG)
        self.batteryax.set_facecolor(BG)
        self.batteryax.tick_params(colors="white")
        self.batteryax.xaxis.label.set_color("white")
        self.batteryax.yaxis.label.set_color("white")
        self.batteryax.spines["bottom"].set_color("white")
        self.batteryax.spines["left"].set_color("white")
        self.batteryax.spines["top"].set_color("white")
        self.batteryax.spines["right"].set_color("white")

        #Materials diagram

        self.materialfg, self.materialax = plt.subplots(figsize=(1,1), dpi=100)
        wedges, texts, autotexts = self.materialax.pie([], colors=["cyan", "yellow", "green"], autopct="%1.1f%%", startangle=90)
        self.materialax.set_aspect("equal")
        self.materialax.legend(wedges, ["Kék Ásvány", "Sárga Ásvány", "Zöld Ásvány"], title="Ásványok", loc="center left", bbox_to_anchor=(1, 0, 0.5, 1))
        self.materialfg.patch.set_facecolor(BG)
        self.materialax.set_facecolor(BG)
        self.materialcanvas = FigureCanvasTkAgg(self.materialfg, master=self.materialframe)
        self.materialcanvas.get_tk_widget().pack(fill="both", expand=True)

        def on_close(self):
            plt.close("all")
            self.main.destroy()
            self.main.quit()

        self.main.protocol("WM_DELETE_WINDOW", lambda: on_close(self))

        self.refleshprg(False)

    def refleshprg(self, refles):
        global logsize
        refleshtime:int = 5
        if logsize < os.path.getsize(self.selectedlogfile):
            backend(self.selectedlogfile)
            global battery, time, materialB, materialG, materialY
            batteryax = self.batteryax

            line = batteryax.lines[0]
            ydata = line.get_ydata()
            if type(ydata) != list:
                ydata.tolist()

            if len(battery) > len(ydata):
                new_y = battery[len(ydata):]
                new_x = time[len(ydata):]
                line.set_ydata(list(ydata) + new_y)
                line.set_xdata(list(line.get_xdata()) + new_x)
                batteryax.set_xticks(time)
                self.batterycanvas.draw()

            self.materialax.clear()
            wedges, texts, autotexts = self.materialax.pie([materialB, materialY, materialG], colors=["cyan", "yellow", "green"], autopct="%1.1f%%", startangle=90)
            self.materialax.set_aspect("equal")
            self.materialax.legend(wedges, ["Kék Ásvány", "Sárga Ásvány", "Zöld Ásvány"], title="Ásványok", loc="center left", bbox_to_anchor=(1, 0, 0.5, 1))
            self.materialfg.patch.set_facecolor("#2e3237")
            self.materialax.set_facecolor("#2e3237")

            self.materialcanvas.draw()

            if self.refleshing == False or refles == True:
                self.main.after(refleshtime * 1000, lambda: self.refleshprg(True))
                self.refleshing = True
        else:
            if self.refleshing == False or refles == True:
                self.main.after(refleshtime * 1000, lambda: self.refleshprg(True))
                self.refleshing = True

main = tk.Tk()


window = Logexplorer(main, "log")

main.mainloop()