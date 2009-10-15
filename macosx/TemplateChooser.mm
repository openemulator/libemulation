
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <string>

#import "OEParser.h"

#import "TemplateChooser.h"
#import "ChooserItem.h"
#import "Document.h"

@implementation TemplateChooser

- (id) init
{
	self = [super init];
	
	if (self)
	{
		groups = [[NSMutableDictionary alloc] init];
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *templatesPath = [resourcePath
								   stringByAppendingPathComponent:@"templates"];
		[self addTemplatesFromPath:templatesPath
						 groupName:nil];
		
		NSString *userTemplatesPath = [TEMPLATE_FOLDER stringByExpandingTildeInPath];
		[self addTemplatesFromPath:userTemplatesPath
						 groupName:NSLocalizedString(@"My Templates",
													 @"My Templates")];
		
		groupNames = [NSMutableArray arrayWithArray:[[groups allKeys]
													 sortedArrayUsingSelector:
													 @selector(compare:)]];
		[groupNames retain];
	}
	
	return self;
}

- (void) dealloc
{
	[groupNames release];
	[groups release];
	
	if (selectedGroup)
		[selectedGroup release];
	
	[super dealloc];
}

- (void) setDelegate:(id)theDelegate
{
	templateChooserDelegate = theDelegate;
}

- (void) addTemplatesFromPath:(NSString *) path
					groupName:(NSString *) theGroupName
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	NSError *error;
	NSArray *templateFilenames = [[NSFileManager defaultManager]
								  contentsOfDirectoryAtPath:path
								  error:&error];
	
	int templatesCount = [templateFilenames count];
	for (int i = 0; i < templatesCount; i++)
	{
		NSString *templateFilename = [templateFilenames objectAtIndex:i];
		NSString *templatePath = [path stringByAppendingPathComponent:templateFilename];
		string templatePathString = string([templatePath UTF8String]);
		OEParser parser(templatePathString);
		if (!parser.isOpen())
			continue;
		
		NSString *label = [templateFilename stringByDeletingPathExtension];
		
		OEDMLInfo *dmlInfo = parser.getDMLInfo();
		NSString *imageName = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *description = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *groupName = [NSString stringWithUTF8String:dmlInfo->group.c_str()];
		
		if (theGroupName)
			groupName = theGroupName;
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		NSString *imagePath = [imagesPath stringByAppendingPathComponent:imageName];
		ChooserItem *item = [[ChooserItem alloc] initWithItem:templatePath
														label:label
													imagePath:imagePath
												  description:description];
		if (item)
			[item autorelease];
		
		[[groups objectForKey:groupName] addObject:item];
	}
}

- (void) updateSelectedGroup
{
	if (selectedGroup)
	{
		[selectedGroup release];
		selectedGroup = nil;
	}
	
	int row = [fOutlineView selectedRow];
	if (row != -1)
	{
		selectedGroup = [groupNames objectAtIndex:row];
		[selectedGroup retain];
	}
}

- (void) populateOutlineView:(id) outlineView
			  andChooserView:(id) chooserView
{
	fOutlineView = [outlineView retain];
	fChooserView = [chooserView retain];
	
	outlineMessagesDisabled = YES;
	[fOutlineView setDataSource:self];
	[fOutlineView setDelegate:self];
	[fOutlineView reloadData];
	
	NSSize aSize;
	aSize.width = 96;
	aSize.height = 64;
	NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
						   [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
						   [NSColor blackColor], NSForegroundColorAttributeName,
						   nil];
	NSDictionary *hAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
							[NSFont messageFontOfSize:11.0f], NSFontAttributeName,
							[NSColor whiteColor], NSForegroundColorAttributeName,
							nil];	
	[fChooserView setAllowsEmptySelection:NO];
	[fChooserView setAllowsMultipleSelection:NO];
	[fChooserView setCellSize:aSize];
	[fChooserView setCellsStyleMask:IKCellsStyleTitled];
	[fChooserView setDataSource:self];
	[fChooserView setDelegate:self];
	[fChooserView setValue:attrs
					forKey:IKImageBrowserCellsTitleAttributesKey];
	[fChooserView setValue:hAttrs
					forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
	outlineMessagesDisabled = NO;
	
	chooserMessagesDisabled = YES;
	[self updateSelectedGroup];
	[fChooserView reloadData];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
				 byExtendingSelection:NO];
	chooserMessagesDisabled = NO;
}

- (void) selectItemWithItemPath:(NSString *) itemPath
{
	int lastGroupIndex = 0;
	int lastChooserIndex = 0;
	
	int groupsCount = [groupNames count];
	for (int i = 0; i < groupsCount; i++)
	{
		NSString *groupName = [groupNames objectAtIndex:i];
		NSArray *templates = [groups objectForKey:groupName];
		
		int templatesCount = [templates count]; 
		for (int j = 0; j < templatesCount; j++)
		{
			ChooserItem *item = [templates objectAtIndex:j];
			if ([[item itemPath] compare:itemPath] == NSOrderedSame)
			{
				lastGroupIndex = i;
				lastChooserIndex = j;
			}
		}
	}
	
	outlineMessagesDisabled = YES;
	[fOutlineView selectRowIndexes:[NSIndexSet indexSet]
			  byExtendingSelection:NO];
	[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:lastGroupIndex]
			  byExtendingSelection:NO];
	outlineMessagesDisabled = NO;
	
	chooserMessagesDisabled = YES;
	[fChooserView reloadData];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:lastChooserIndex]
				 byExtendingSelection:NO];
	[fChooserView scrollIndexToVisible:lastChooserIndex];
	chooserMessagesDisabled = NO;
}

- (id) outlineView:(NSOutlineView *) outlineView child:(NSInteger) index ofItem:(id) item
{
	if (!item)
		return [groupNames objectAtIndex:index];
	
	return nil;
}

- (BOOL) outlineView:(NSOutlineView *) outlineView isItemExpandable:(id) item
{
	return NO;
}

- (NSInteger) outlineView:(NSOutlineView *) outlineView numberOfChildrenOfItem:(id) item
{
	if (!item)
		return [groupNames count];
	
	return 0;
}

- (id)outlineView: (NSOutlineView *) outlineView
objectValueForTableColumn:(NSTableColumn *) tableColumn
		   byItem:(id) item
{
	return item;
}

- (void) outlineViewSelectionDidChange:(NSNotification *) notification
{
	[self updateSelectedGroup];
	
	if (!outlineMessagesDisabled)
	{
		chooserMessagesDisabled = YES;
		[fChooserView reloadData];
		[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
					 byExtendingSelection:NO];
		chooserMessagesDisabled = NO;
		
		[self imageBrowserSelectionDidChange:nil];
	}
}

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *) aBrowser
{
	if (!selectedGroup)
		return 0;
	
	return [[groups objectForKey:selectedGroup] count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger) index
{
	if (!selectedGroup)
		return nil;
	
	return [[groups objectForKey:selectedGroup] objectAtIndex:index];
}

- (void) imageBrowserSelectionDidChange:(IKImageBrowserView *) aBrowser
{
	if (!chooserMessagesDisabled)
	{
		if ([templateChooserDelegate respondsToSelector:
			 @selector(templateChooserSelectionDidChange:)])
			[templateChooserDelegate templateChooserSelectionDidChange:self];
	}
}

- (void) imageBrowser:(IKImageBrowserView *) aBrowser
cellWasDoubleClickedAtIndex:(NSUInteger) index
{
	if ([templateChooserDelegate respondsToSelector:
		  @selector(templateChooserWasDoubleClicked:)])
		[templateChooserDelegate templateChooserWasDoubleClicked:self];
}

- (NSString *) selectedItemPath
{
	int index = [[fChooserView selectionIndexes] firstIndex];
	if (index == NSNotFound)
		return nil;
	
	ChooserItem *item = [self imageBrowser:fChooserView itemAtIndex:index];
	
	return [[[item itemPath] copy] autorelease];
}

@end