/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "output.h"
#include "output_p.h"

namespace GreenIsland {

namespace Client {

static Output::Subpixel toSubpixel(const QtWayland::wl_output::subpixel &value)
{
    switch (value) {
    case QtWayland::wl_output::subpixel_unknown:
        return Output::SubpixelUnknown;
    case QtWayland::wl_output::subpixel_none:
        return Output::SubpixelNone;
    case QtWayland::wl_output::subpixel_horizontal_rgb:
        return Output::SubpixelHorizontalRgb;
    case QtWayland::wl_output::subpixel_horizontal_bgr:
        return Output::SubpixelHorizontalBgr;
    case QtWayland::wl_output::subpixel_vertical_rgb:
        return Output::SubpixelVerticalRgb;
    case QtWayland::wl_output::subpixel_vertical_bgr:
        return Output::SubpixelVerticalBgr;
    default:
        break;
    }

    return Output::SubpixelUnknown;
}

static Output::Transform toTransform(const QtWayland::wl_output::transform &value)
{
    switch (value) {
    case QtWayland::wl_output::transform_90:
        return Output::Transform90;
    case QtWayland::wl_output::transform_180:
        return Output::Transform180;
    case QtWayland::wl_output::transform_270:
        return Output::Transform270;
    case QtWayland::wl_output::transform_flipped:
        return Output::TransformFlipped;
    case QtWayland::wl_output::transform_flipped_90:
        return Output::TransformFlipped90;
    case QtWayland::wl_output::transform_flipped_180:
        return Output::TransformFlipped180;
    case QtWayland::wl_output::transform_flipped_270:
        return Output::TransformFlipped270;
    default:
        break;
    }

    return Output::TransformNormal;
}

/*
 * OutputPrivate
 */

OutputPrivate::OutputPrivate()
    : QtWayland::wl_output()
    , scale(1)
    , currentMode(modes.end())
    , subpixelValue(Output::SubpixelUnknown)
    , transformValue(Output::TransformNormal)
{
}

Output *OutputPrivate::fromWlOutput(struct ::wl_output *output)
{
    QtWayland::wl_output *wlOutput =
            static_cast<QtWayland::wl_output *>(wl_output_get_user_data(output));
    return static_cast<OutputPrivate *>(wlOutput)->q_func();
}

void OutputPrivate::output_geometry(int32_t x, int32_t y,
                                    int32_t physical_width, int32_t physical_height,
                                    int32_t _subpixel, const QString &make,
                                    const QString &_model, int32_t _transform)
{
    Q_Q(Output);

    if (manufacturer != make) {
        manufacturer = make;
        Q_EMIT q->manufacturerChanged();
    }

    if (model != _model) {
        model = _model;
        Q_EMIT q->modelChanged();
    }

    if (position.x() != x || position.y() != y) {
        position = QPoint(x, y);
        Q_EMIT q->positionChanged();
    }

    if (physicalSize.width() != physical_width || physicalSize.height() != physical_height) {
        physicalSize = QSize(physical_width, physical_height);
        Q_EMIT q->physicalSizeChanged();
    }

    auto newSubpixel = toSubpixel(static_cast<QtWayland::wl_output::subpixel>(_subpixel));
    if (subpixelValue != newSubpixel) {
        subpixelValue = newSubpixel;
        Q_EMIT q->subpixelChanged();
    }

    auto newTransform = toTransform(static_cast<QtWayland::wl_output::transform>(_transform));
    if (transformValue != newTransform) {
        transformValue = newTransform;
        Q_EMIT q->transformChanged();
    }
}

void OutputPrivate::output_mode(uint32_t flags, int32_t width, int32_t height,
                                int32_t refresh)
{
    Q_Q(Output);

    Output::Mode mode;
    mode.flags = 0;
    mode.size = QSize(width, height);
    mode.refreshRate = qreal(refresh) / 1000;

    if (flags & QtWayland::wl_output::mode_current)
        mode.flags |= Output::CurrentMode;
    if (flags & QtWayland::wl_output::mode_preferred)
        mode.flags |= Output::PreferredMode;

    // Append the mode
    auto it = modes.insert(modes.end(), mode);

    bool found = false;

    // Current mode has been changed
    if (flags & QtWayland::wl_output::mode_current) {
        auto curIt = modes.begin();
        while (curIt != it) {
            Output::Mode &m = (*it);

            // Remove the current mode flag from the previous mode
            if (m.flags.testFlag(Output::CurrentMode)) {
                m.flags &= ~Output::ModeFlags(Output::CurrentMode);
                Q_EMIT q->modeChanged(m);
                Q_EMIT q->sizeChanged();
                Q_EMIT q->geometryChanged();
                Q_EMIT q->refreshRateChanged();
            }

            // Replace if it has the same size and refresh rate
            if (m.size == mode.size && m.refreshRate == mode.refreshRate) {
                curIt = modes.erase(curIt);
                found = true;
            } else {
                ++it;
            }
        }

        currentMode = it;
    }

    if (found) {
        Q_EMIT q->modeChanged(mode);
        Q_EMIT q->sizeChanged();
        Q_EMIT q->geometryChanged();
        Q_EMIT q->refreshRateChanged();
    } else {
        Q_EMIT q->modeAdded(mode);
    }
}

void OutputPrivate::output_done()
{
    // The done event is sent by the server in order to make
    // properties changes feel like they are atomic. We are not
    // particularly interested in this.
}

void OutputPrivate::output_scale(int32_t factor)
{
    Q_Q(Output);

    if (scale != factor) {
        scale = factor;
        Q_EMIT q->scaleChanged();
    }
}

/*
 * Output
 */

Output::Output(QObject *parent)
    : QObject(*new OutputPrivate(), parent)
{
}

QString Output::manufacturer() const
{
    Q_D(const Output);
    return d->manufacturer;
}

QString Output::model() const
{
    Q_D(const Output);
    return d->model;
}

QPoint Output::position() const
{
    Q_D(const Output);
    return d->position;
}

QSize Output::size() const
{
    Q_D(const Output);
    if (d->currentMode == d->modes.end())
        return QSize();
    return d->currentMode->size;
}

QRect Output::geometry() const
{
    Q_D(const Output);
    if (d->currentMode == d->modes.end())
        return QRect();
    return QRect(d->position, d->currentMode->size);
}

qreal Output::refreshRate() const
{
    Q_D(const Output);
    if (d->currentMode == d->modes.end())
        return 0;
    return d->currentMode->refreshRate;
}

QSize Output::physicalSize() const
{
    Q_D(const Output);
    return d->physicalSize;
}

qint32 Output::scale() const
{
    Q_D(const Output);
    return d->scale;
}

QList<Output::Mode> Output::modes() const
{
    Q_D(const Output);
    return d->modes.toList();
}

Output::Subpixel Output::subpixel() const
{
    Q_D(const Output);
    return d->subpixelValue;
}

Output::Transform Output::transform() const
{
    Q_D(const Output);
    return d->transformValue;
}

QByteArray Output::interfaceName()
{
    return QByteArrayLiteral("wl_output");
}

} // namespace Client

} // namespace GreenIsland

#include "moc_output.cpp"