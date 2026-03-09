import tkinter as tk
from tkinter import *
import customtkinter as Ctk
import os
from customtkinter import *
from CTkListbox import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import sys

Ctk.set_appearance_mode("dark")
Ctk.set_default_color_theme("blue")


def clearwidget(main):
    for widget in main.winfo_children():
        widget.destroy()


class Logexplorer:
    def __init__(self, main:Ctk.CTk, logfile: str):
        self.main = main
        self.logfile = logfile

        self.main.title("Dashboard - Log kiválasztása")
        self.main.geometry("400x400")
        self.main.resizable(True, True)
        self.main.iconbitmap("src/img/dashboardicon.ico")

        self.frame = CTkFrame(self.main, corner_radius=10, fg_color="transparent")
        self.frame.pack(fill=BOTH, expand=True, padx=10, pady=10)

        self.header = CTkFrame(self.frame, fg_color="transparent")
        self.header.pack(fill=X, pady=(0, 10))

        self.header.columnconfigure(0, weight=1)

        CTkLabel(
            self.header,
            text="Log kiválasztása",
            font=CTkFont(family="Courier New", size=18, weight="bold")
        ).grid(row=0, column=0, sticky="w")

        CTkLabel(
            self.header,
            text="Kérjük válasszon egy dátumot!",
            font=CTkFont(size=13)
        ).grid(row=1, column=0, sticky="w")

        CTkButton(
            self.header,
            text="🔄 Újratöltés",
            font=CTkFont(family="Courier New", size=14),
            cursor="hand2",
            command=lambda: self.reflesh(),
            corner_radius=8
        ).grid(row=0, rowspan=2, column=1, padx=(10, 0), sticky="e")


        self.frame2 = CTkFrame(self.frame, corner_radius=10)
        self.frame2.pack(fill=BOTH, expand=True)

        self.listbox = CTkListbox(self.frame2, multiple_selection=False)

        self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)

        self.errorframe = CTkFrame(self.frame2, fg_color="transparent")

        CTkLabel(
            self.errorframe,
            text="ⓘ",
            font=CTkFont(family="Courier New", size=26, weight="bold")
        ).pack(side=LEFT, padx=5)

        CTkLabel(
            self.errorframe,
            text="Sajnos nem található log.",
            font=CTkFont(family="Courier New", size=18, weight="bold")
        ).pack(side=LEFT, padx=5)

        self.donebtn = CTkButton(
            self.frame2,
            text="Kész",
            font=CTkFont(family="Courier New", size=15),
            cursor="hand2",
            command=lambda: self.done(logfile + "/" + str(self.listbox.get()) + ".log"),
            corner_radius=8
        )

        self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

        if len(os.listdir(logfile)) <= 0:
            self.listbox.pack_forget()
            self.donebtn.pack_forget()
            self.errorframe.pack(expand=True)
        else:
            self.errorframe.pack_forget()
            self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)
            self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

            self.listbox.delete(0, END)
            for file in os.listdir(logfile):
                self.listbox.insert(END, file[:-4])

    def reflesh(self):

        if len(os.listdir(self.logfile)) <= 0:
            self.listbox.pack_forget()
            self.donebtn.pack_forget()
            self.errorframe.pack(expand=True)
        else:
            self.errorframe.pack_forget()
            self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)
            self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

            self.listbox.delete(0, END)
            for file in os.listdir(self.logfile):
                self.listbox.insert(END, file[:-4])

    def done(self, logfile):
        if logfile != None and len(logfile) > 0:
            global window
            window = DashboardUI(self.main, logfile)
        else:
            print("Kötelező kijelölni 1 elemet!")


logline:int = 1
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
events:list = []


def backend(logfile):
    global logsize, logline, time, battery, positionY, positionX, speed, distance, materialB, materialY, materialG
    if logfile != None and len(logfile) > 0 and logsize < os.path.getsize(logfile):
        with open(logfile, "r") as fullog:
            lines = fullog.readlines()
            materialch:str = ""
            speedch:str = ""

            for i in range(logline, len(lines)):

                fullline:list = lines[i].strip().split(";")

                positionX.append(int(fullline[0].split(",")[0]))
                positionY.append(int(fullline[0].split(",")[1]))
                battery.append(int(fullline[1]))
                speed.append(int(fullline[2]))
                distance.append(int(fullline[2]))

                if len(fullline) >= 4:

                    if fullline[3].lower() == "y":
                        materialY += 1
                        materialch = "| ⛏ Sárga ásvány"

                    elif fullline[3].lower() == "b":
                        materialB += 1
                        materialch = "| ⛏ Kék ásvány"

                    elif fullline[3].lower() == "g":
                        materialG += 1
                        materialch = "| ⛏ Zöld ásvány"

                if len(time) == 0:
                    time.append(0)
                else:
                    time.append(time[len(time) - 1] + 30)

                if int(fullline[2]) == 3:
                    speedch = "Gyors"
                elif int(fullline[2]) == 2:
                    speedch = "Normál"
                elif int(fullline[2]) == 1:
                    speedch = "Lassú"
                elif int(fullline[2]) == 0:
                    speedch = "Áll"

                events.append(
                    f"[{int(time[len(time)-1])//60}:{int(time[len(time)-1])%60:02d}] ⚡{battery[len(battery)-1]}% | 🚀 {speedch} {materialch}"
                )
                materialch = ""

            fullog.close()

            logline = len(lines)
            logsize = os.path.getsize(logfile)


class DashboardUI:
    def __init__(self, main:Ctk.CTk, selectedlogfile:str):
        self.main:Ctk.CTk = main
        self.selectedlogfile:str = selectedlogfile

        clearwidget(self.main)

        self.main.title("Dashboard")
        self.main.geometry("1390x800")
        self.main.resizable(True, True)

        BG = "#2e3237"
        BG2 = "#262626"

        self.frame = CTkFrame(self.main, fg_color=BG, corner_radius=0)
        self.frame.pack(fill="both", expand=True)

        self.header = CTkFrame(self.frame, fg_color="transparent")
        self.header.pack(fill="x", padx=15, pady=15)

        CTkButton(
            self.header,
            text="🔄 Újratöltés",
            font=CTkFont(family="Courier New", size=20),
            cursor="hand2",
            corner_radius=8,
            command=lambda: self.refleshprg(False)
        ).pack(side="right")

        CTkLabel(
            self.header,
            text="Dashboard",
            font=CTkFont(family="Courier New", size=30, weight="bold")
        ).pack(anchor="w")

        CTkLabel(
            self.header,
            text="Logok betöltése 5 másodpercenként.",
            font=CTkFont(family="Courier New", size=16)
        ).pack(anchor="w")
       
        self.bodyframe = CTkFrame(self.frame, fg_color="transparent")
        self.bodyframe.pack(fill="both", expand=True, padx=10, pady=10)

        for i in range(3):
            self.bodyframe.columnconfigure(i, weight=1)
            if i < 2:
                self.bodyframe.rowconfigure(i, weight=1)

        def create_panel(parent, title, row, col, colspan=1, rfbtn=False, BG=BG2):
            outer = CTkFrame(
                parent,
                fg_color=BG,
                border_width=2,
                corner_radius=10
            )

            outer.grid(
                row=row,
                column=col,
                columnspan=colspan,
                sticky="nsew",
                padx=6,
                pady=6
            )

            outer.rowconfigure(1, weight=1)
            outer.columnconfigure(0, weight=1)

            CTkLabel(
                outer,
                text=title,
                font=CTkFont(family="Courier New", size=18, weight="bold")
            ).grid(row=0, column=0, sticky="ew", pady=(5, 0), padx=(5,5))


            if rfbtn:
                CTkButton(
                    outer,
                    text="Teljese nézet",
                    font=CTkFont(family="Courier New", size=14, weight="bold"),
                    cursor="hand2",
                    corner_radius=8
                ).grid(row=0, column=1, pady=(5, 0), padx=(5,5))

            graph_area = CTkFrame(
                outer,
                fg_color="transparent"
            )

            graph_area.grid(
                row=1,
                column=0,
                columnspan=2,
                sticky="nsew",
                padx=5,
                pady=5
            )

            return graph_area
        
        self.dataframe = create_panel(
            self.bodyframe,
            "Egyéb Adatok",
            0, 0,
            BG=BG2
        )

        self.batteryframe = create_panel(
            self.bodyframe,
            "Akkumulátor Töltöttség (%)",
            0, 1,
            rfbtn=True,
            BG=BG2
        )

        self.speedframe = create_panel(
            self.bodyframe,
            "Sebesség",
            0, 2,
            rfbtn=True,
            BG=BG2
        )

        self.materialframe = create_panel(
            self.bodyframe,
            "Gyűjtött Ásványok",
            1, 0,
            BG=BG2
        )

        self.positionframe = create_panel(
            self.bodyframe,
            "Rover Pozíciója",
            1, 1,
            BG=BG2
        )

        self.logframe = create_panel(
            self.bodyframe,
            "Eseménynapló",
            1, 2,
            BG=BG2
        )

        self.materialheaderframe = CTkFrame(
            self.materialframe,
            fg_color="transparent"
        )

        self.materialheaderframe.pack(expand=True)
        self.allmaterial = tk.StringVar(value="Összes ásvány: 0 db")

        CTkLabel(
            self.materialheaderframe,
            textvariable=self.allmaterial,
            font=CTkFont(family="Courier New", size=18, weight="bold"),
            text_color="white"
        ).pack()

        self.positionheaderframe = CTkFrame(
            self.positionframe,
            fg_color="transparent"
        )

        self.positionheaderframe.pack(expand=True)
        self.distancestartvar = tk.StringVar(value="Távolság a kiinduló ponttól: 0 blokk")

        CTkLabel(
            self.positionheaderframe,
            textvariable=self.distancestartvar,
            font=CTkFont(family="Courier New", size=15, weight="bold"),
            text_color="white"
        ).pack()

        for i in range(2):
            self.dataframe.columnconfigure(i, weight=1)
            if i < 1:
                self.dataframe.rowconfigure(i, weight=1)
    
        self.timeframe = create_panel(
            self.dataframe,
            "Szimulációs Idő",
            0, 0,
            BG="#171717"
        )

        self.timevar = tk.StringVar(value="0:00")
        self.subtimevar = tk.StringVar(value="Nappal")
        
        CTkLabel(
            self.timeframe,
            textvariable=self.timevar,
            font=CTkFont(family="Courier New", size=25, weight="bold")
            ).pack()
        
        CTkLabel(
            self.timeframe,
            textvariable=self.subtimevar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        self.datadistanceframe = create_panel(
            self.dataframe,
            "Megtett távolság",
            0, 1,
            BG="#171717"
        )

        self.distancevar = tk.StringVar(value="0 blokk")

        CTkLabel(
            self.datadistanceframe,
            textvariable=self.distancevar,
            font=CTkFont(family="Courier New", size=25, weight="bold")
        ).pack()

        self.databatteryframe = create_panel(
            self.dataframe,
            "Energia Fogyasztás",
            1, 0,
            BG="#171717"
        )

        self.datamindtvar = tk.StringVar(value="Minimum: 0%")
        self.dataatlagdtvar = tk.StringVar(value="Átlag: 0%")
        self.datamaxdtvar = tk.StringVar(value="Maximum: 0%")

        CTkLabel(
            self.databatteryframe,
            textvariable=self.datamindtvar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        CTkLabel(
            self.databatteryframe,
            textvariable=self.dataatlagdtvar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        CTkLabel(
            self.databatteryframe,
            textvariable=self.datamaxdtvar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        self.dataaiframe = create_panel(
            self.dataframe,
            "AI Hatékonyság",
            1, 1,
            BG="#171717"
        )

        self.dataaiblokkvar = tk.StringVar(value="Ásvány / blokk: 0")
        self.dataaienergiavar = tk.StringVar(value="Ásvány / energia: 0")

        CTkLabel(
            self.dataaiframe,
            textvariable=self.dataaiblokkvar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        CTkLabel(
            self.dataaiframe,
            textvariable=self.dataaienergiavar,
            font=CTkFont(family="Courier New", size=20, weight="bold")
        ).pack()

        def createbardiagram(ylabel:str, xlabel:str, frame:CTkFrame):
            fg, ax = plt.subplots(figsize=(4, 3), dpi=100)

            fg.subplots_adjust(
                left=0.125,
                right=0.9,
                bottom=0.25,
                top=0.9
            )

            bar = ax.bar([], [], width=20, linewidth=1, color="lightgreen")

            ax.set_ylim(0, 100)
            ax.set_ylabel(ylabel)
            ax.set_xlabel(xlabel)
            ax.grid(True, linestyle="--", linewidth=0.5)

            canvas = FigureCanvasTkAgg(
                fg,
                master=frame
            )

            canvas.get_tk_widget().pack(
                fill="both",
                expand=True
            )

            fg.patch.set_facecolor(BG2)
            ax.set_facecolor(BG2)

            ax.tick_params(colors="white")

            ax.xaxis.label.set_color("white")
            ax.yaxis.label.set_color("white")
            ax.spines["bottom"].set_color("white")
            ax.spines["left"].set_color("white")
            ax.spines["top"].set_color("white")
            ax.spines["right"].set_color("white")
            
            return fg, ax, canvas, bar

        def createlinediagram(ylabel:str, xlabel:str, yticklables:list, yticks:list, frame:CTkFrame):
            fg, ax = plt.subplots(figsize=(4, 3), dpi=100)

            fg.subplots_adjust(
                left=0.2,
                right=0.9,
                bottom=0.25,
                top=0.9
            )

            ax.plot([], [], marker="o")

            ax.set_yticks(yticks)
            ax.set_yticklabels(yticklables)
            ax.set_ylabel(ylabel)
            ax.set_xlabel(xlabel)
            ax.grid(True, linestyle="--", linewidth=0.5)

            canvas = FigureCanvasTkAgg(
                fg,
                master=frame
            )

            canvas.get_tk_widget().pack(
                fill="both",
                expand=True
            )

            fg.patch.set_facecolor(BG2)
            ax.set_facecolor(BG2)

            ax.tick_params(colors="white")

            ax.xaxis.label.set_color("white")
            ax.yaxis.label.set_color("white")

            ax.spines["bottom"].set_color("white")
            ax.spines["left"].set_color("white")
            ax.spines["top"].set_color("white")
            ax.spines["right"].set_color("white")

            return fg, ax, canvas
        
        def createpiediagram(frame:CTkFrame):
            fg, ax = plt.subplots(
                    figsize=(4, 3),
                    dpi=100
                    )

            fg.subplots_adjust(
                left=0.1,
                right=0.6,
                bottom=0.2,
                top=0.9
            )

            canvas = FigureCanvasTkAgg(
                fg,
                master=frame
            )

            canvas.get_tk_widget().pack(
                fill="both",
                expand=True
            )

            fg.patch.set_facecolor(BG2)

            return fg, ax, canvas
        
        def createcoordinatesdiagram(frame:CTkFrame):
            fg, ax = plt.subplots(
                figsize=(4, 3),
                dpi=100
            )

            fg.subplots_adjust(
                left=0.1,
                right=0.6,
                bottom=0.2,
                top=0.9
            )

            canvas = FigureCanvasTkAgg(
                fg,
                master=frame
            )

            canvas.get_tk_widget().pack(
                fill="both",
                expand=True
            )

            fg.patch.set_facecolor(BG2)

            x = [5,-3,-6,0,0,4]
            y = [3,1,-4,-3,0,-5]

            n=['A','B','C','D','E','F']

            ax.scatter(x, y)

            for i, txt in enumerate(n):
                ax.annotate(txt, (x[i], y[i]))

            ax.grid(True, linestyle="--", linewidth=0.5)

            ax.set_facecolor(BG2)

            ax.tick_params(colors="white")

            ax.xaxis.label.set_color("white")
            ax.yaxis.label.set_color("white")

            ax.spines["bottom"].set_color("white")
            ax.spines["left"].set_color("white")
            ax.spines["top"].set_color("white")
            ax.spines["right"].set_color("white")

            return fg, ax, canvas

        self.batteryfg, self.batteryax, self.batterycanvas, self.batterybar = createbardiagram("Töltöttség (%)", "Idő (óra:perc)", self.batteryframe)
        self.speedfg, self.speedax, self.speedcanvas = createlinediagram("Sebbesség", "Idő (óra:perc)", ["Áll", "Lassú", "Normál", "Gyors"], [0, 1, 2, 3], self.speedframe)
        self.materialfg, self.materialax, self.materialcanvas = createpiediagram(self.materialframe)
        self.positionfg, self.positionax, self.positioncanvas = createcoordinatesdiagram(self.positionframe)
        
        self.logframe2 = CTkFrame(
            self.logframe,
            fg_color="transparent"
        )

        self.logframe2.pack(fill="both", expand=True, pady=5, padx=5)

        self.loglsbx = CTkListbox(
                self.logframe2,
                font=CTkFont(family="Courier New", size=15),
                multiple_selection=False,
                hover=False,
                fg_color="#171717"
            )

        self.loglsbx.pack(fill=BOTH, expand=True)

        def on_close(self):

            plt.close("all")

            self.main.destroy()

            self.main.quit()

        self.main.protocol(
            "WM_DELETE_WINDOW",
            lambda: on_close(self)
        )

        self.refleshprg(True)

    def refleshprg(self, refles):
        global logsize, battery, time, materialB, materialG, materialY, speed, events
        refleshtime:int = 5

        if logsize < os.path.getsize(self.selectedlogfile):
            backend(self.selectedlogfile)
            maxdata:int = 7

            for i in range(len(events)):
                if i >= self.loglsbx.size():
                    self.loglsbx.insert(END, events[i])


            def linediagram(ax, datas, maxdata:int, canvas):
                line = ax.lines[0]

                xdata = list(line.get_xdata())
                ydata = list(line.get_ydata())

                if type(ydata) != list:
                    ydata.tolist()

                if len(datas) > len(ydata):

                    new_y = datas[len(ydata):]
                    new_x = time[len(xdata):]

                    xdata = (xdata + new_x)[-maxdata:]
                    ydata = (ydata + new_y)[-maxdata:]

                    line.set_data(xdata, ydata)

                    ax.set_xticks(xdata)

                    labels = [
                        f"{int(i)//60}:{int(i)%60:02d}"
                        for i in xdata
                    ]

                    ax.set_xticklabels(labels)

                    ax.relim()
                    ax.autoscale_view()
                    canvas.draw_idle()
            
            def bardiagram(ax, datas, maxdata, canvas, bar):
                xdata = time[-maxdata:]
                ydata = datas[-maxdata:]
                for i in bar:
                    i.remove()
                bar = ax.bar(xdata, ydata, width=20, linewidth=1, color="lightgreen")

                labels = [
                    f"{int(i)//60}:{int(i)%60:02d}"
                    for i in xdata
                ]

                ax.set_xticks(xdata)
                ax.set_xticklabels(labels)

                ax.relim()
                ax.autoscale_view()
                canvas.draw_idle()

                return bar
            
            def piediagram(ax, canvas, datas:list, datascolor:list, legendtitles:list):
                ax.clear()

                wedges, texts, autotexts = ax.pie(
                    datas,
                    colors=datascolor,
                    shadow=True,
                    autopct="%1.1f%%",
                    startangle=90,
                    radius=1.2
                )

                for autotext in autotexts:

                    autotext.set_color('black')
                    autotext.set_fontweight('bold')

                ax.set_aspect("equal")

                ax.legend(
                    wedges,
                    legendtitles,
                    loc="center left",
                    bbox_to_anchor=(1, 0, 0.5, 1),
                    facecolor="#2e3237",
                    labelcolor="white",
                    fancybox=False,
                    frameon=False,
                    alignment="left",
                    handlelength=1.5,
                    handleheight=1.5
                )

                ax.set_facecolor("#2e3237")
                canvas.draw()

            self.allmaterial.set(value="Összes ásvány: " + str(materialB + materialY + materialG) + " db")
            self.distancestartvar.set(value="Távolság a kiinduló ponttól: " + str("ERROR") + " blokk")
            self.batterybar = bardiagram(self.batteryax, battery, maxdata, self.batterycanvas, self.batterybar)
            linediagram(self.speedax, speed, maxdata, self.speedcanvas)
            piediagram(self.materialax, self.materialcanvas, [materialB, materialY, materialG], ["cyan", "yellow", "green"], ["Kék Ásvány", "Sárga Ásvány", "Zöld Ásvány"])           
            self.timevar.set(value=f"{int(time[len(time)-1])//60}:{int(time[len(time)-1])%60:02d}")

            if (int(time[-1]) % 1440) < 960:
                self.subtimevar.set(value="Nappal")
            else:
                self.subtimevar.set(value="Éjszaka")
            
            distance = 0
            for i in speed:
                distance += int(i)

            self.distancevar.set(value=str(distance) + " blokk")

            minbt = int(battery[0])
            maxbt = int(battery[0])
            atlagbt = 0
            buffer = 0

            for i in battery:
                if int(i) < minbt:
                    minbt = int(i)
                elif int(i) > maxbt:
                    maxbt = int(i)
                
                buffer += int(i)

            atlagbt = round(buffer/len(battery))
            buffer = 0
            usedallbattery = 0

            for i in range(1, len(battery)):
                buffer = battery[i-1] - battery[i]
                if buffer > 0:
                    usedallbattery += buffer

            self.datamindtvar.set(value="Minimum: " + str(minbt) + "%")
            self.dataatlagdtvar.set(value="Átlag: " + str(atlagbt) + "%")
            self.datamaxdtvar.set(value="Maximum: " + str(maxbt) + "%")
            self.dataaiblokkvar.set(value="Ásvány / blokk: " + str(round((materialB + materialY + materialG)/distance, 2)))
            self.dataaienergiavar.set(value="Ásvány / energia: " + str(usedallbattery))
            
            if refles == True:
                self.main.after(
                    refleshtime * 1000,
                    lambda: self.refleshprg(True)
                )

        else:
            if refles == True:
                self.main.after(
                    refleshtime * 1000,
                    lambda: self.refleshprg(True)
                )

main = Ctk.CTk()

if len(sys.argv) != 1:
    window = DashboardUI(main, "log/" + str(sys.argv[1]) + ".log")
else:
    window = Logexplorer(main, "log")

main.mainloop()