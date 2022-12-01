#include "movie.h"

Movie::Movie() {

}

Movie::Movie(const QString &title, const QString &coverURL, const QString &key, const QString &category, const QString &actor, const QString &intro)
    : title(title), coverURL(coverURL), key(key), category(category), actor(actor), intro(intro) {

}
