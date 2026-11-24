# import tkinter as tk
# # from tkinter import ttk

# root = tk.Tk()

# root.geometry('400x400+100+100')
# root.title("spell cheker")

# # tk.Label(root, text="Name:").grid(column=0, row=0, sticky="w")
# # tk.Entry(root).grid(column=1, row=0)
# # tk.Button(root, text="OK").grid(column=0, row=1, sticky="e")

# textBox = tk.Text(root, font=("Courier", 16))
# textBox.pack()

# root.mainloop()

# import tkinter as tk

# root = tk.Tk()

# # Create the context menu
# menu = tk.Menu(root, tearoff=0)
# # menu = tk.Menu(root)
# menu.add_command(label="Copy", command=lambda: print("Copy"))
# menu.add_command(label="Paste", command=lambda: print("Paste"))
# menu.add_separator()
# menu.add_command(label="Delete", command=lambda: print("Delete"))

# # Show the menu on right-click
# def show_menu(event):
#     print(event)
#     menu.post(event.x_root, event.y_root)

# root.bind("<Button-3>", show_menu)  # Right-click on Windows/Linux

# root.mainloop()

# import tkinter as tk

# root = tk.Tk()
# root.title("My App")

# def save_file():
#     print("File saved!")

# def open_file():
#     print("File opened!")

# # Create the top menu bar
# menubar = tk.Menu(root)

# # Create File menu
# file_menu = tk.Menu(menubar, tearoff=0)

# file_menu.add_command(label="Open", command=open_file)
# file_menu.add_command(label="Save", command=save_file)
# file_menu.add_separator()
# file_menu.add_command(label="Exit", command=root.quit)

# # Attach File menu to the menu bar
# menubar.add_cascade(label="File", menu=file_menu)

# # Attach menu bar to the window
# root.config(menu=menubar)

# root.mainloop()

import tkinter as tk

root = tk.Tk()

text = tk.StringVar()

def changed(*args):
    print("Text changed:", text.get())
    # print(args)

text.trace_add("write", changed)

entry = tk.Entry(root, textvariable=text)
entry.pack()

root.mainloop()