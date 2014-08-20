// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once


/**
 * Interface for tool tips.
 */
class IToolTip
{
public:

	/**
	 * Gets the widget that this tool tip represents.
	 *
	 * @return The tool tip widget.
	 */
	virtual TSharedRef<class SWidget> AsWidget( ) = 0;

	/**
	 * Gets the tool tip's content widget.
	 *
	 * @return The content widget.
	 */
	virtual TSharedRef<SWidget> GetContentWidget( ) = 0;

	/**
	 * Sets the tool tip's content widget.
	 *
	 * @param InContentWidget The new content widget to set.
	 */
	virtual void SetContentWidget( const TSharedRef<SWidget>& InContentWidget ) = 0;

	/**
	 * Checks whether this tool tip has no content to display right now.
	 *
	 * @return true if the tool tip has no content to display, false otherwise.
	 */
	virtual bool IsEmpty( ) const = 0;

	/**
	 * Checks whether this tool tip can be made interactive by the user (by holding Ctrl).
	 *
	 * @return true if it is an interactive tool tip, false otherwise.
	 */
	virtual bool IsInteractive( ) const = 0;

public:

	/** Virtual destructor. */
	virtual ~IToolTip( ) { }
};
