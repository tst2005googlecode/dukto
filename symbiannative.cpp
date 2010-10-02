#include "symbiannative.h"

#if defined(Q_WS_S60)
#include <eikedwin.h>
#include <baclipb.h>
#include <s32ucmp.h>
#endif

QString SymbianNative::getClipboardText()
{

#if defined(Q_WS_S60)

    QString ret = "";
    CClipboard *cb = CClipboard::NewForReadingLC(CCoeEnv::Static()->FsSession());
    TStreamId stid = cb->StreamDictionary().At(KClipboardUidTypePlainText);
    if (KNullStreamId != stid)
    {
        RStoreReadStream stream;
        stream.OpenLC(cb->Store(), stid);
        const TInt32 size = stream.ReadInt32L();
        HBufC *buf = HBufC::NewLC(size);
        buf->Des().SetLength(size);

        TUnicodeExpander e;
        TMemoryUnicodeSink sink(&buf->Des()[0]);
        e.ExpandL(sink, stream, size);

        ret = QString::fromUtf16(buf->Ptr(), buf->Length());

        stream.Close();
        CleanupStack::Pop(); // stream.OpenLC
        CleanupStack::PopAndDestroy(buf);
    }
    CleanupStack::PopAndDestroy(cb);

    return ret;

#else

    return "";

#endif
}

void SymbianNative::setClipboardText(QString text)
{
#if defined(Q_WS_S60)

    TPtrC aText (static_cast<const TUint16*>(text.utf16()), text.length());

    //void WriteToClipboardL(RFs &aFs, const TDesC & aText)
    CClipboard *cb = CClipboard::NewForWritingLC(CCoeEnv::Static()->FsSession());

    RStoreWriteStream stream;
    TStreamId stid = stream.CreateLC(cb->Store());
    stream.WriteInt32L(aText.Length());

    TUnicodeCompressor c;
    TMemoryUnicodeSource source(aText.Ptr());
    TInt bytes(0);
    TInt words(0);
    c.CompressL(stream, source, KMaxTInt, aText.Length(), &bytes, &words);

    stream.WriteInt8L(0); // magic command! :)

    stream.CommitL();
    cb->StreamDictionary().AssignL(KClipboardUidTypePlainText, stid);
    cb->CommitL();

    stream.Close();
    CleanupStack::PopAndDestroy(); // stream.CreateLC
    CleanupStack::PopAndDestroy(cb);

#endif

}
