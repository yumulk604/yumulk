import ui
import ime
import localeInfo

class GoldCoinDialog(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.eventAccept = None

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def LoadDialog(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/PickMoneyDialog.py")
        except:
            import exception
            exception.Abort("GoldCoinDialog.LoadDialog.LoadScript")

        try:
            self.board = self.GetChild("board")
            self.pickValueEditLine = self.GetChild("money_value")
            self.acceptButton = self.GetChild("accept_button")
            self.cancelButton = self.GetChild("cancel_button")
            self.GetChild("max_value").Hide()
        except:
            import exception
            exception.Abort("GoldCoinDialog.LoadDialog.BindObject")

        self.board.SetTitleName(localeInfo.GOLD_COIN_SYSTEM_TITLE)
        self.GetChild("money_text").SetText(localeInfo.GOLD_COIN_SYSTEM_SUBTITLE)

        self.pickValueEditLine.SetReturnEvent(ui.__mem_func__(self.OnAccept))
        self.pickValueEditLine.SetEscapeEvent(ui.__mem_func__(self.Close))
        self.acceptButton.SetEvent(ui.__mem_func__(self.OnAccept))
        self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
        self.board.SetCloseEvent(ui.__mem_func__(self.Close))

    def Destroy(self):
        self.ClearDictionary()
        self.eventAccept = None
        self.pickValueEditLine = None
        self.acceptButton = None
        self.cancelButton = None
        self.board = None

    def SetAcceptEvent(self, event):
        self.eventAccept = event

    def Open(self, maxYang):
        self.pickValueEditLine.SetText("1")
        self.pickValueEditLine.SetFocus()
        self.SetTop()
        self.Show()

    def Close(self):
        self.pickValueEditLine.KillFocus()
        self.Hide()

    def OnAccept(self):
        text = self.pickValueEditLine.GetText()
        if text and text.isdigit():
            money = int(text)
            if money > 0:
                if self.eventAccept:
                    self.eventAccept(money)
        self.Close()
