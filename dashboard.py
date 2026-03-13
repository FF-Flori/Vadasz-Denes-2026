import customtkinter as Ctk
from customtkinter import *
from CTkListbox import *
import sys
from src.DASHBOARD.logexplorer import *
from src.DASHBOARD.mainui import *

Ctk.set_appearance_mode("dark")
Ctk.set_default_color_theme("blue")
main = Ctk.CTk()

if len(sys.argv) != 1:
    DashboardUI(main, "log/" + str(sys.argv[1]) + ".log")
else:
    Logexplorer(main, "log")

main.mainloop()