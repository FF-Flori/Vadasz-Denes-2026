import sys
import os
try:
    import customtkinter as Ctk
    from customtkinter import *

except ImportError:
    current_dir = os.path.dirname(os.path.abspath(__file__))
    libdeps_path = os.path.join(current_dir, 'src', 'libdeps')

    if libdeps_path not in sys.path:
        sys.path.insert(0, libdeps_path)

    import customtkinter as Ctk
    from customtkinter import *

from src.DASHBOARD.logexplorer import *
from src.DASHBOARD.mainui import *

Ctk.set_appearance_mode("dark")
Ctk.set_default_color_theme("blue")
main = Ctk.CTk()

if len(sys.argv) != 1:
    DashboardUI(main, str(sys.argv[1]))
else:
    Logexplorer(main)

main.mainloop()