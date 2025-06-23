import net
import ui
import shop
import item

class OfflineShopDialog(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.board = ui.Board()
        self.board.SetParent(self)
        self.board.SetSize(300, 200)
        self.board.Show()

    def Open(self):
        self.Show()

    def Close(self):
        self.Hide()


offlineShop = None

def OpenOfflineShop():
    global offlineShop
    if not offlineShop:
        offlineShop = OfflineShopDialog()
    offlineShop.Open()

def CloseOfflineShop():
    if offlineShop:
        offlineShop.Close()
