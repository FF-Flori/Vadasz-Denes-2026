import customtkinter as Ctk
from customtkinter import *
from CTkListbox import *

def clearwidget(main:Ctk.CTk):
    for widget in main.winfo_children():
        widget.destroy()