## Blueprint Message Log Plugin for Unreal Engine

Are you still printing plain text to output? Discover Message Log! Create rich interactive messages with different contents with little of blueprint.

Blueprint Message Plugin provides an easy bridge between Blueprints and Message Log API to open an era of interactive logging! 

Each Message Log message represented by sequence of tokens that can be interacted with: hyperlinks, references, actions, images.

Note: plugin is at experimental stage, things may change a lot.

## Features

 * Blueprint wrapper of Message Log API
 * Blueprint wrappers for tokens:
   * FTextToken - plain text
   * FURLToken - hyperlink
   * FUObjectToken - reference to UObject
   * FActorToken - reference to Actor
   * FAssetNameToken - reference to Asset
   * FImageToken - image 
   * FTutorialToken - reference to tutorial asset
   * FDocumentationToken - reference to documentation 
   * FDynamicTextToken - dynamically updating text
   * FActionToken - execute blueprint delegate on click
   * FEditorUtilityToken - activates Blutility Widget
 * Message Slots - reserve a spot and fill it with token afterwards!
 * Custom tokens support

## Unreal Engine Versions

Tested with 5.0, ue5-main and can be modified to work with other engine versions.

## Examples

Minimal "Hello World" can be as simple as this

![](Images/BMP-HelloWorld.png)

You can also build message out of tokens directly

![](Images/BMP-CreateMessage.png)

A simple message with reference to actor in world to locate it in one click

![](Images/BMP-ActorToken.png)

A simple message with executable action (in PIE only)

![](Images/BMP-ActionToken.png)

A simple message with action that activates an Editor Utility Widget

![](Images/BMP-EditorUtilityToken.png)

You don't need to make separate Format nodes as "Add Formatted Text Token" exists

![](Images/BMP-FormattedToken.png)

Slots can be used to further format messages and replace tokens while building

![](Images/BMP-Slots.png)

Message Log View:

![](Images/BMP-MessageLog.png)

## Contributing

Please report any issues with GitHub Issues page for this repository.

If you want to suggest changes, improvements or updates to the plugin use GitHub Pull Requests.

## License

BlueprintMessagePlugin is available under the MIT license. See the LICENSE file for more info.

---

Special Thanks to Unreal Slackers Community Discord and #cpp 
