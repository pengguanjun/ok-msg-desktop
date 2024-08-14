/*
 * Copyright (c) 2022 船山信息 chuanshaninfo.com
 * The project is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PubL v2. You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef ICONSET_H
#define ICONSET_H

#include <QHash>
#include <QImage>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QRegExp>
#include <QSharedData>
#include <QString>
#include <QStringList>

class Anim;
class QFileInfo;
class QIcon;

class Impix {
public:
    Impix();
    Impix(const QPixmap&);
    Impix(const QImage&);

    void unload();
    bool isNull() const;

    const QPixmap& pixmap() const;
    const QImage& image() const;
    void setPixmap(const QPixmap&);
    void setImage(const QImage&);

    operator const QPixmap&() const { return pixmap(); }
    operator const QImage&() const { return image(); }
    Impix& operator=(const QPixmap& from) {
        setPixmap(from);
        return *this;
    }
    Impix& operator=(const QImage& from) {
        setImage(from);
        return *this;
    }

    QSize size() const;

    bool loadFromData(const QByteArray&);

private:
    class Private : public QSharedData {
    public:
        QPixmap* pixmap;
        QImage image;

        Private() { pixmap = nullptr; }

        Private(const Private& from) : QSharedData(from) {
            pixmap = from.pixmap ? new QPixmap(*from.pixmap) : nullptr;
            image = from.image;
        }

        ~Private() { unload(); }

        void unload() {
            if (pixmap) delete pixmap;
            pixmap = nullptr;
            image = QImage();
        }
    };

    mutable QSharedDataPointer<Private> d;
};

class PsiIcon : public QObject {
    Q_OBJECT
public:
    PsiIcon();
    PsiIcon(const PsiIcon&);
    ~PsiIcon();

    PsiIcon& operator=(const PsiIcon&);

    //!
    //! Returns impix().pixmap().
    // operator const QPixmap &() const { return impix().pixmap(); }

    //!
    //! Returns impix().image().
    // operator const QImage &() const { return impix().image(); }

    //!
    //! see icon().
    // operator const QIcon &() const { return icon(); }

    virtual bool isAnimated() const;
    virtual QPixmap pixmap(const QSize& desiredSize = QSize()) const;
    virtual QImage image(const QSize& desiredSize = QSize()) const;
    virtual QIcon icon() const;
    const QByteArray& raw() const;
    QSize size(const QSize& desiredSize = QSize()) const;
    bool isScalable() const;
    const QString& mimeType() const;

    virtual const Impix& impix() const;
    virtual const Impix& frameImpix() const;
    void setImpix(const Impix&, bool doDetach = true);

    const Anim* anim() const;
    void setAnim(const Anim&, bool doDetach = true);
    void removeAnim(bool doDetach = true);

    virtual int frameNumber() const;

    virtual const QString& name() const;
    void setName(const QString&);

    const QRegExp& regExp() const;
    void setRegExp(const QRegExp&);

    struct IconText {
        IconText(QString _lang, QString _text) : lang(_lang), text(_text) {}

        QString lang;
        QString text;
    };

    const QList<IconText>& text() const;
    void setText(const QList<IconText>&);

    QString defaultText() const;

    const QString& sound() const;
    void setSound(const QString&);

    bool blockSignals(bool);
    bool loadFromData(const QString& mime, const QByteArray&, bool isAnimation,
                      bool isScalable = false);

    void stripFirstAnimFrame();

    virtual PsiIcon* copy() const;
    void detach();

signals:
    void pixmapChanged();
    void iconModified();

public slots:
    virtual void activated(bool playSound = true);  // it just has been inserted in the text, or now
                                                    // it's being displayed by some widget. icon
                                                    // should play sound and start animation

    virtual void stop();  // this icon is no more displaying. stop animation

public:
    class Private;

private:
    QSharedDataPointer<Private> d;
};

class Iconset {
public:
    enum class Format { Psi, KdeEmoticons };

    Iconset();
    Iconset(const Iconset&);
    ~Iconset();

    Iconset& operator=(const Iconset&);
    Iconset& operator+=(const Iconset&);

    void clear();
    int count() const;

    bool load(const QString& dir, Format format = Format::Psi);

    const PsiIcon* icon(const QString&) const;
    void setIcon(const QString&, const PsiIcon&);
    void removeIcon(const QString&);

    const QString& id() const;
    const QString& name() const;
    const QString& version() const;
    const QString& description() const;
    const QStringList& authors() const;
    const QString& creation() const;
    const QString& homeUrl() const;
    const int& iconSize() const;

    const QString& fileName() const;
    void setFileName(const QString&);

    void setInformation(const Iconset& from);

    const QHash<QString, QString> info() const;
    void setInfo(const QHash<QString, QString>&);

    QListIterator<PsiIcon*> iterator() const;
    QList<PsiIcon*>::const_iterator begin() const;
    QList<PsiIcon*>::const_iterator end() const;

    void addToFactory() const;
    void removeFromFactory() const;

    static bool isSourceAllowed(const QFileInfo& fi);
    static void setSoundPrefs(QString unpackPath, QObject* receiver, const char* slot);

    Iconset copy() const;
    void detach();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

class IconsetFactory {
public:
    static void reset();

    static PsiIcon icon(const QString& name);
    static inline PsiIcon icon(const char* name) {
        return icon(QString(QLatin1String(name)));
    }  // optimization
    static QPixmap iconPixmap(const QString& name, const QSize desiredSize = QSize());
    static inline QPixmap iconPixmap(const QString& name, int desiredSize) {
        return iconPixmap(name, QSize(desiredSize, desiredSize));
    }

    static const PsiIcon* iconPtr(const QString& name);
    static const QStringList icons();

    static const QByteArray raw(const QString& name);
};

#endif  // ICONSET_H
