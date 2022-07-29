## Blueprint Message Plugin for Unreal Engine

Blueprint Message Plugin created to provide an easy bridge between Blueprints and Message Log. 

Each Message Log message represented by sequence of tokens that can be interacted with.

## Features

 * Blueprint wrapper of Message Log API
 * Blueprint wrappers for tokens:
   * FTextToken - plain text
   * FURLToken - hyperlink
   * FSeverityToken - message severity switch
   * FUObjectToken - reference to UObject
   * FActorToken - reference to Actor
   * FAssetNameToken - reference to Asset
   * FImageToken - image 
   * FTutorialToken - reference to tutorial asset
   * FDocumentationToken - reference to documentation 
   * FDynamicTextToken (Soon...)
   * FActionToken (Soon...)
   * FEditorUtilityToken - reference to Blutility Widget/Action/Task
 * Message Slots - reserve a spot and fill it with token afterwards!
 * Custom blueprint graph nodes for Create Message and Add Token (Soon...)

## Unreal Engine Versions

Tested with 5.0, ue5-main and can be modified to work with other engine versions.

## Contributing

Please report any issues with GitHub Issues page for this repository.

If you want to suggest changes, improvements or updates to the plugin use GitHub Pull Requests.

## License

BlueprintMessagePlugin is available under the MIT license. See the LICENSE file for more info.

---

Special Thanks to Unreal Slackers Community Discord and #cpp 
