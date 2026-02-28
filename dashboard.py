import customtkinter as tk
import os
from customtkinter import *
from CTkListbox import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import numpy as np

tk.set_appearance_mode("dark")
tk.set_default_color_theme("blue")


def clearwidget(main):
    for widget in main.winfo_children():
        widget.destroy()


class Logexplorer:
    def __init__(self, main:tk.CTk, logfile: str):
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

        self.label = CTkLabel(
            self.header,
            text="Log kiválasztása",
            font=CTkFont(family="Courier New", size=18, weight="bold")
        )
        self.label.grid(row=0, column=0, sticky="w")

        self.sublabel = CTkLabel(
            self.header,
            text="Kérjük válasszon egy dátumot!",
            font=CTkFont(size=13)
        )
        self.sublabel.grid(row=1, column=0, sticky="w")

        self.refbtn = CTkButton(
            self.header,
            text="🔄 Újratöltés",
            font=CTkFont(family="Courier New", size=14),
            cursor="hand2",
            command=lambda: self.reflesh(),
            corner_radius=8
        )
        self.refbtn.grid(row=0, rowspan=2, column=1, padx=(10, 0), sticky="e")

        self.frame2 = CTkFrame(self.frame, corner_radius=10)
        self.frame2.pack(fill=BOTH, expand=True)

        self.listbox = CTkListbox(self.frame2)

        self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)

        self.errorframe = CTkFrame(self.frame2, fg_color="transparent")

        self.erroriconlb = CTkLabel(
            self.errorframe,
            text="ⓘ",
            font=CTkFont(family="Courier New", size=26, weight="bold")
        )

        self.errorlb = CTkLabel(
            self.errorframe,
            text="Sajnos nem található log.",
            font=CTkFont(family="Courier New", size=18, weight="bold")
        )

        self.erroriconlb.pack(side=LEFT, padx=5)
        self.errorlb.pack(side=LEFT, padx=5)

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


logline: int = 1
logsize: int = 0
time: list = []
battery: list = []
positionY: list = []
positionX: list = []
speed: list = []
distance: list = []
materialB: int = 0
materialY: int = 0
materialG: int = 0


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

                fullline: list = lines[i].strip().split(";")

                positionX.append(int(fullline[0].split(",")[0]))
                positionY.append(int(fullline[0].split(",")[1]))
                battery.append(int(fullline[1]))
                speed.append(int(fullline[2]))
                distance.append(int(fullline[2]))

                if len(fullline) >= 4:

                    if fullline[3].lower() == "y":
                        materialY += 1

                    elif fullline[3].lower() == "b":
                        materialB += 1

                    elif fullline[3].lower() == "g":
                        materialG += 1

                if len(time) == 0:
                    time.append(0)
                else:
                    time.append(time[len(time) - 1] + 30)

            fullog.close()

            logline = len(lines)
            logsize = os.path.getsize(logfile)


class DashboardUI:
    def __init__(self, main, selectedlogfile):

        self.main = main
        self.selectedlogfile = selectedlogfile

        clearwidget(self.main)

        self.main.title("Dashboard")
        self.main.geometry("1200x800")
        self.main.resizable(True, True)

        BG = "#2e3237"
        BG2 = "#262626"

        self.frame = CTkFrame(self.main, fg_color=BG, corner_radius=0)
        self.frame.pack(fill="both", expand=True)

        self.header = CTkFrame(self.frame, fg_color="transparent")
        self.header.pack(fill="x", padx=15, pady=15)

        self.refbtn = CTkButton(
            self.header,
            text="🔄 Újratöltés",
            font=CTkFont(family="Courier New", size=20),
            cursor="hand2",
            corner_radius=8,
            command=lambda: self.refleshprg(False)
        )

        self.refbtn.pack(side="right")

        self.title = CTkLabel(
            self.header,
            text="Dashboard",
            font=CTkFont(family="Courier New", size=30, weight="bold")
        )
        self.title.pack(anchor="w")

        self.subtitle = CTkLabel(
            self.header,
            text="Logok betöltése 5 másodpercenként.",
            font=CTkFont(family="Courier New", size=16)
        )
        self.subtitle.pack(anchor="w")

        self.bodyframe = CTkFrame(self.frame, fg_color="transparent")
        self.bodyframe.pack(fill="both", expand=True, padx=10, pady=10)

        for i in range(3):
            self.bodyframe.columnconfigure(i, weight=1)

        for i in range(2):
            self.bodyframe.rowconfigure(i, weight=1)

        def create_panel(parent, title, row, col, colspan=1):

            outer = CTkFrame(
                parent,
                fg_color=BG2,
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

            title_label = CTkLabel(
                outer,
                text=title,
                font=CTkFont(family="Courier New", size=14, weight="bold")
            )

            title_label.grid(row=0, column=0, sticky="ew", pady=(5, 0), padx=(5,5))

            graph_area = CTkFrame(
                outer,
                fg_color="transparent"
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

        self.batteryfg, self.batteryax = plt.subplots(figsize=(4, 3), dpi=100)

        self.batteryfg.subplots_adjust(
            left=0.125,
            right=0.9,
            bottom=0.2,
            top=0.9
        )

        self.batteryax.plot([], [], marker="o", color="lightgreen")

        self.batteryax.set_ylim(0, 100)
        self.batteryax.set_ylabel("Töltöttség (%)")
        self.batteryax.set_xlabel("Idő (perc)")
        self.batteryax.grid(True, linestyle="--", linewidth=0.5)

        self.batterycanvas = FigureCanvasTkAgg(
            self.batteryfg,
            master=self.batteryframe
        )

        self.batterycanvas.get_tk_widget().pack(
            fill="both",
            expand=True
        )

        self.batteryfg.patch.set_facecolor(BG2)
        self.batteryax.set_facecolor(BG2)

        self.batteryax.tick_params(colors="white")

        self.batteryax.xaxis.label.set_color("white")
        self.batteryax.yaxis.label.set_color("white")

        self.batteryax.spines["bottom"].set_color("white")
        self.batteryax.spines["left"].set_color("white")
        self.batteryax.spines["top"].set_color("white")
        self.batteryax.spines["right"].set_color("white")

        self.materialfg, self.materialax = plt.subplots(
            figsize=(4, 3),
            dpi=100
        )

        self.materialfg.subplots_adjust(
            left=0.1,
            right=0.6,
            bottom=0.2,
            top=0.9
        )

        self.materialcanvas = FigureCanvasTkAgg(
            self.materialfg,
            master=self.materialframe
        )

        self.materialcanvas.get_tk_widget().pack(
            fill="both",
            expand=True
        )

        self.materialfg.patch.set_facecolor(BG2)

        def on_close(self):

            plt.close("all")

            self.main.destroy()

            self.main.quit()

        self.main.protocol(
            "WM_DELETE_WINDOW",
            lambda: on_close(self)
        )

        self.refleshprg(False)

    def refleshprg(self, refles):

        global logsize

        refleshtime: int = 5

        if logsize < os.path.getsize(self.selectedlogfile):

            backend(self.selectedlogfile)

            global battery
            global time
            global materialB
            global materialG
            global materialY

            batteryax = self.batteryax

            line = batteryax.lines[0]

            ydata = line.get_ydata()

            if type(ydata) != list:
                ydata.tolist()

            if len(battery) > len(ydata):

                new_y = battery[len(ydata):]
                new_x = time[len(ydata):]

                line.set_ydata(
                    list(ydata) + new_y
                )

                line.set_xdata(
                    list(line.get_xdata()) + new_x
                )

                batteryax.set_xticks(time)

                self.batterycanvas.draw()

            self.materialax.clear()

            wedges, texts, autotexts = self.materialax.pie(
                [materialB, materialY, materialG],
                colors=["cyan", "yellow", "green"],
                shadow=True,
                autopct="%1.1f%%",
                startangle=90,
                radius=1.2
            )

            for autotext in autotexts:

                autotext.set_color('black')
                autotext.set_fontweight('bold')

            self.materialax.set_aspect("equal")

            self.materialax.legend(
                wedges,
                ["Kék Ásvány", "Sárga Ásvány", "Zöld Ásvány"],
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

            self.materialax.set_facecolor("#2e3237")

            self.materialcanvas.draw()

            if refles == True:

                self.main.after(
                    refleshtime * 1000,
                    lambda: self.refleshprg(True)
                )

                self.refleshing = True

        else:

            if refles == True:

                self.main.after(
                    refleshtime * 1000,
                    lambda: self.refleshprg(True)
                )

                self.refleshing = True


main = tk.CTk()

window = Logexplorer(main, "log")

main.mainloop()