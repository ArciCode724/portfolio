import tkinter as tk
from tkinter import messagebox

def show_word(event):
    # Get position in the Text widget
    index = text_box.index(f"@{event.x},{event.y}")

    # Get the word boundaries
    start = text_box.index(f"{index} wordstart")
    end = text_box.index(f"{index} wordend")

    # Get the word
    word = text_box.get(start, end)

    text_box.tag_config("red", foreground="red")

    text_box.tag_add("red", start, end)

    # if word.strip():
    #     messagebox.showinfo("Word", word)

root = tk.Tk()
root.title("Right click word popup")

text_box = tk.Text(root, width=50, height=10)
text_box.pack(padx=10, pady=10)

text_box.insert("1.0", "Move your mouse over a word and right click it.")

# Right mouse button
text_box.bind("<Button-3>", show_word)

root.mainloop()
