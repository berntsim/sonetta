#include "player.h"

#include <QCoreApplication>

namespace sp = Spotinetta;


namespace Sonetta {

Player::Player(Spotinetta::Session *session, AudioOutput *output, QObject *parent)
    :   QObject(parent), m_session(session), m_shuffle(false), m_repeat(false),
      m_output(output), m_queue(new QueueModel(this))
{
    Q_ASSERT(session != nullptr);

    m_watcher = new sp::TrackWatcher(session, this);
    m_positionTimer = new QTimer(this);

    // Set up connections
    connect(session, &sp::Session::endOfTrack, this, &Player::next);
    connect(m_positionTimer, &QTimer::timeout, this, &Player::positionChanged);

    // This ensures a track is eventually played whether it's loaded
    // or not
    connect(m_watcher, &sp::TrackWatcher::loaded, [this] {
        play(m_watcher->watched());
    });;

    m_positionTimer->start(150);
}

bool Player::shuffle() const
{
    return m_shuffle;
}

bool Player::repeat() const
{
    return m_repeat;
}

int Player::position() const
{
    return m_output.isNull() ? 0 : m_output->position();
}

sp::Track Player::track() const
{
    return m_watcher->watched();
}

void Player::setShuffle(bool enable)
{
    if (m_shuffle != enable)
    {
        m_shuffle = enable;
        emit shuffleChanged();
    }
}

void Player::setRepeat(bool enable)
{
    if (m_repeat != enable)
    {
        m_repeat = enable;
        emit repeatChanged();
    }
}

QObject * Player::queue() const
{
    return m_queue;
}

void Player::play(const Spotinetta::Track &track)
{
    if (this->track().isValid())
    {
        // If currently playing a track, unload it first
        m_session->unload();

        // Clear buffers
        if (!m_output.isNull())
        {
            m_output->reset();
        }
    }

    if (track != this->track())
    {
        m_watcher->watch(track);
        emit trackChanged();
    }

    if (track.isLoaded())
    {
        if (m_session->load(track))
        {
            m_session->play();

            if (!m_output.isNull())
                m_output->resetPosition(0);
        }
    }

    // If track is not loaded, it will be played when it is loaded,
    // as the watcher's loaded signal is connected to play(track)
}

void Player::enqueue(const Spotinetta::Track &track)
{
    // Assumes track is loaded, might want to add watcher and enqueue when ready
    // or it may be better to add watch functionality to QueueModel
    m_queue->enqueue(track);
}

void Player::play()
{
    m_session->play();
}

void Player::playPause()
{
    if (m_session->playbackState() == sp::Session::PlaybackState::Paused)
    {
        play();
    }
    else
    {
        pause();
    }
}

void Player::pause()
{
    m_session->pause();
}

void Player::next()
{
    sp::Track track;
    // Try to get an available track from queue
    while (!m_queue->isEmpty() && track.availability(m_session) != sp::Track::Availability::Available)
        track = m_queue->dequeue();

    if (track.isValid())
    {
        play(track);
    }
    else {
        pause();
        m_watcher->watch(sp::Track());
    }

    // Emit outside if because of end of track situations
    emit trackChanged();
}


}
