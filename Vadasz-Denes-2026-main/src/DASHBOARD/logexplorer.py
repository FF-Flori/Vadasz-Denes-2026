import customtkinter as Ctk
from customtkinter import *
from CTkListbox import *

class Logexplorer:
    def __init__(self, main:Ctk.CTk):
        self.main = main
        self.mainlog = "log"

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
            command=lambda: self.done(str(self.listbox.get())),
            corner_radius=8
        )

        self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

        if len(os.listdir(self.mainlog)) <= 0:
            self.listbox.pack_forget()
            self.donebtn.pack_forget()
            self.errorframe.pack(expand=True)
        else:
            self.errorframe.pack_forget()
            self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)
            self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

            self.listbox.delete(0, END)
            for file in os.listdir(self.mainlog):
                self.listbox.insert(END, file[:-4])

    def reflesh(self):

        if len(os.listdir(self.mainlog)) <= 0:
            self.listbox.pack_forget()
            self.donebtn.pack_forget()
            self.errorframe.pack(expand=True)
        else:
            self.errorframe.pack_forget()
            self.listbox.pack(fill=BOTH, expand=True, padx=5, pady=5)
            self.donebtn.pack(side=BOTTOM, fill=X, padx=5, pady=5)

            self.listbox.delete(0, END)
            for file in os.listdir(self.mainlog):
                self.listbox.insert(END, file[:-4])

    def done(self, logfile):
        from src.DASHBOARD.mainui import DashboardUI
        DashboardUI(self.main, logfile)