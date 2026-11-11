/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "Slideshow.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <utility>

#include <QRandomGenerator>

namespace
{

using pictorio::folder_scanner::Sorting;
using pictorio::folder_scanner::SortOrder;
using pictorio::slideshow::Slideshow;

Sorting sortingFor(Slideshow::Order order)
{
    switch (order)
    {
    case Slideshow::Order::ByName:
        return {SortOrder::Name};
    case Slideshow::Order::ByDate:
        return {SortOrder::Date};
    case Slideshow::Order::Random:
        return {SortOrder::Random, QRandomGenerator::global()->generate()};
    }

    return {};
}

// How each order is written in the settings
constexpr std::array<std::pair<Slideshow::Order, const char*>, 3> orderNames{{
    {Slideshow::Order::ByName, "name"},
    {Slideshow::Order::ByDate, "date"},
    {Slideshow::Order::Random, "random"},
}};

QString nameOf(Slideshow::Order order)
{
    const auto entry = std::ranges::find(orderNames, order, &std::pair<Slideshow::Order, const char*>::first);
    return QString::fromLatin1(entry->second);
}

// Unknown names (e.g. from a newer version) give the default order.
Slideshow::Order orderNamed(const QString& name)
{
    const auto entry = std::ranges::find_if(
        orderNames, [&](const auto& orderName) { return name == QLatin1StringView(orderName.second); });
    return entry != orderNames.end() ? entry->first : Slideshow::Order::ByName;
}

}  // namespace

namespace pictorio::slideshow
{

Slideshow::Slideshow(navigator::Navigator* navigator, settings::Settings* settings, QObject* parent)
    : QObject(parent)
    , m_navigator(navigator)
    , m_settings(settings)
    , m_order(orderNamed(m_settings->slideshowOrder().value_or(QString())))
{
    applyIntervalSeconds(m_settings->slideshowIntervalSeconds().value_or(defaultIntervalSeconds));
    connect(&m_timer, &QTimer::timeout, this, &Slideshow::showNextImage);
    connect(m_navigator, &navigator::Navigator::imageRequested, this, &Slideshow::restartIntervalIfShowing);
}

Slideshow::~Slideshow() = default;

void Slideshow::start(Order order)
{
    if (order != m_order)
    {
        m_order = order;
        m_settings->setSlideshowOrder(nameOf(order));
        Q_EMIT orderChanged();
    }

    m_navigator->setSorting(sortingFor(order));
    m_navigator->setWrapAround(true);
    m_running = true;
    m_paused = false;
    m_timer.start();
    Q_EMIT stateChanged();
}

void Slideshow::stop()
{
    if (!m_running)
    {
        return;
    }

    m_timer.stop();
    m_running = false;
    m_paused = false;
    m_navigator->setSorting({});
    m_navigator->setWrapAround(false);
    Q_EMIT stateChanged();
}

void Slideshow::pause()
{
    if (m_running && !m_paused)
    {
        m_timer.stop();
        m_paused = true;
        Q_EMIT stateChanged();
    }
}

void Slideshow::resume()
{
    if (m_running && m_paused)
    {
        m_paused = false;
        m_timer.start();
        Q_EMIT stateChanged();
    }
}

bool Slideshow::isRunning() const
{
    return m_running;
}

bool Slideshow::isPaused() const
{
    return m_paused;
}

int Slideshow::intervalSeconds() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(m_timer.intervalAsDuration()).count());
}

void Slideshow::setIntervalSeconds(int intervalSeconds)
{
    const int previousSeconds = this->intervalSeconds();
    applyIntervalSeconds(intervalSeconds);
    if (this->intervalSeconds() != previousSeconds)
    {
        m_settings->setSlideshowIntervalSeconds(this->intervalSeconds());
        Q_EMIT intervalSecondsChanged();
    }
}

Slideshow::Order Slideshow::order() const
{
    return m_order;
}

void Slideshow::applyIntervalSeconds(int intervalSeconds)
{
    const int limitedSeconds = std::clamp(intervalSeconds, minimumIntervalSeconds, maximumIntervalSeconds);
    m_timer.setInterval(std::chrono::seconds(limitedSeconds));  // takes effect from the next image
}

int Slideshow::minimumIntervalSecondsForQml() const
{
    return minimumIntervalSeconds;
}

int Slideshow::maximumIntervalSecondsForQml() const
{
    return maximumIntervalSeconds;
}

void Slideshow::showNextImage()
{
    // Nothing to do while the folder is still being listed; the next tick tries again.
    if (m_navigator->canGoToNext())
    {
        m_navigator->goToNext();
    }
}

void Slideshow::restartIntervalIfShowing()
{
    if (m_running && !m_paused)
    {
        m_timer.start();
    }
}

}  // namespace pictorio::slideshow
