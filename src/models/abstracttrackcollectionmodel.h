#pragma once

#include <QAbstractListModel>
#include <Spotinetta/Spotinetta>

namespace Sonetta {

/*!
 * \brief The AbstractTrackCollectionModel class defines common roleNames and
 *  necessary model class implementations for models representing collections of tracks.
 */


class AbstractTrackCollectionModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum Role {
        TrackRole = Qt::UserRole,
        ArtistsRole,
        AlbumRole,
        IsLoadedRole,
        IsValidRole,
        NameRole,
        ArtistNamesRole,
        AlbumNameRole,
        DurationRole,
        PopularityRole,

        // Used in subclasses
        LastTrackCollectionModelRole
    };

    explicit AbstractTrackCollectionModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

protected:
    virtual Spotinetta::Track getTrackAt(int index) const = 0;
    virtual int getTrackCount() const = 0;

    void updateTrackData(int first, int last = -1);
    void updateAlbumData(int first, int last = -1);
    void updateArtistData(int first, int last = -1);
    void updateData(int first, int last = -1);

private:
    Q_DISABLE_COPY(AbstractTrackCollectionModel)
};

}
