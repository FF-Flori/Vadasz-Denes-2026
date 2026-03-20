import sys
import os
try:
    import customtkinter as Ctk
    from customtkinter import *
    from CTkListbox import *

except ImportError:
    current_dir = os.path.dirname(os.path.abspath(__file__))
    libdeps_path = os.path.join(current_dir, '..', 'libdeps')

    if libdeps_path not in sys.path:
        sys.path.insert(0, libdeps_path)

    import customtkinter as Ctk
    from customtkinter import *
    from CTkListbox import *

def clearwidget(main:Ctk.CTk):
    for widget in main.winfo_children():
        widget.destroy()