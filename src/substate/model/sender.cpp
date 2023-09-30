#include "sender.h"
#include "sender_p.h"

namespace Substate {

    SenderPrivate::SenderPrivate() {
        is_clearing = false;
    }

    SenderPrivate::~SenderPrivate() {
        is_clearing = true;
        for (const auto &sub : std::as_const(subscribers))
            sub->m_sender = nullptr;
    }

    void SenderPrivate::init() {
    }

    /*!
        \class Sender
        \brief Sender is used to broadcast notifications to subscribers.
    */

    /*!
        Destructor.
    */
    Sender::~Sender() {
    }

    /*!
        Adds a subscriber.
    */
    void Sender::addSubscriber(Subscriber *sub) {
        Q_D(Sender);

        std::unique_lock<std::shared_mutex> lock(d->shared_lock);

        auto it = d->subscriberIndexes.find(sub);
        if (it != d->subscriberIndexes.end())
            return;
        d->subscriberIndexes.insert(
            std::make_pair(sub, d->subscribers.insert(d->subscribers.end(), sub)));

        sub->m_sender = this;
    }

    /*!
        Removes a subscriber.
    */
    void Sender::removeSubscriber(Subscriber *sub) {
        Q_D(Sender);

        std::unique_lock<std::shared_mutex> lock(d->shared_lock);

        auto it = d->subscriberIndexes.find(sub);
        if (it == d->subscriberIndexes.end())
            return;
        d->subscribers.erase(it->second);
        d->subscriberIndexes.erase(it);

        sub->m_sender = nullptr;
    }

    /*!
        Returns true if the sender is in destruction.
    */
    bool Sender::isBeingDestroyed() const {
        Q_D(const Sender);
        return d->is_clearing;
    }

    /*!
        Notifies all subscribers of the notification.
    */
    void Sender::dispatch(Notification *n) {
        Q_D(Sender);

        std::shared_lock<std::shared_mutex> lock(d->shared_lock);
        for (const auto &sub : std::as_const(d->subscribers)) {
            sub->notified(n);
        }
    }

    /*!
        \internal
    */
    Sender::Sender(SenderPrivate &d) : d_ptr(&d) {
        d.q_ptr = this;
        d.init();
    }

    /*!
        \class Subscriber
        \brief Subscriber receives notifications from Sender.
    */

    /*!
        Constructor.
    */
    Subscriber::Subscriber() : m_sender(nullptr) {
    }

    /*!
        Destructor.
    */
    Subscriber::~Subscriber() {
        if (m_sender)
            m_sender->removeSubscriber(this);
    }

    /*!
        \fn Sender *Subscriber::sender() const

        Returns the sender.
    */

    /*!
        \fn void Subscriber::notified(Notification *n)

        Processes the current notification.
    */

    /*!
        \class Notification
        \brief Notification is the message transmission medium in Substate framework.
    */

    /*!
        Constructor.
    */
    Notification::Notification(int type) : t(type) {
    }

    /*!
        Destructor.
    */
    Notification::~Notification() {
    }

}