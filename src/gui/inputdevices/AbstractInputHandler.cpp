//
// Created by ulrich on 06.04.19.
//

#include "AbstractInputHandler.h"
#include "InputContext.h"

AbstractInputHandler::AbstractInputHandler(InputContext* inputContext)
{
	XOJ_INIT_TYPE(AbstractInputHandler);

	this->inputContext = inputContext;
	this->pressureSensitivity = inputContext->getSettings()->isPresureSensitivity();
}

AbstractInputHandler::~AbstractInputHandler()
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	XOJ_RELEASE_TYPE(AbstractInputHandler);
}

void AbstractInputHandler::block(bool block)
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	this->blocked = block;
	this->onBlock();
}

bool AbstractInputHandler::isBlocked()
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	return this->blocked;
}

bool AbstractInputHandler::handle(GdkEvent* event)
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	if (!this->blocked)
	{
		return this->handleImpl(event);
	} else {
		return true;
	}
}

/**
 * Get Page at current position
 *
 * @return page or NULL if none
 */
XojPageView* AbstractInputHandler::getPageAtCurrentPosition(GdkEvent* event)
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	if (event == nullptr)
	{
		return nullptr;
	}

	gdouble eventX, eventY;
	gdk_event_get_coords(event, &eventX, &eventY);

	//take scroll offset into account
	this->inputContext->getScrollHandling()->translate(eventX, eventY);

	GtkXournal* xournal = this->inputContext->getXournal();

	double x = eventX + xournal->x;
	double y = eventY + xournal->y;

	return xournal->layout->getViewAt(x,y);
}

/**
 * Get input data relative to current input page
 */
PositionInputData AbstractInputHandler::getInputDataRelativeToCurrentPage(XojPageView* page, GdkEvent* event)
{
	XOJ_CHECK_TYPE(AbstractInputHandler);

	GtkXournal* xournal = inputContext->getXournal();

	gdouble eventX, eventY;
	gdk_event_get_coords(event, &eventX, &eventY);

	//take scroll offset into account
	this->inputContext->getScrollHandling()->translate(eventX, eventY);

	PositionInputData pos;
	pos.x = eventX - page->getX() - xournal->x;
	pos.y = eventY - page->getY() - xournal->y;
	pos.pressure = Point::NO_PRESSURE;

	if (pressureSensitivity)
	{
		gdk_event_get_axis(event, GDK_AXIS_PRESSURE, &pos.pressure);
	}

	pos.state = this->inputContext->getModifierState();
	pos.timestamp = gdk_event_get_time(event);

	return pos;
}

void AbstractInputHandler::onBlock()
{
	XOJ_CHECK_TYPE(AbstractInputHandler);
}
