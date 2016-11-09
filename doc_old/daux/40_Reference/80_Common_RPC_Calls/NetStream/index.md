NetStream commands follow a fairly consistent convention. A NetStream command is issued, and upon completion, an `onStatus` command is sent back. All NetStream actions take place with a sequence number of 0.

## Syntax ##
An `onStatus` response has the following structure:

```
String: "onStatus"
Number: 0
Null
Object: {
String:     level: ...
String:     code: ...
String:     description: ...
        }
```

## Parameters ##
#### response[0] ####
> **Type:** String
>
> The name of the command, in this case, "onStatus".

#### response[1] ####
> **Type:** Number
>
> The sequence number for this command. Always 0 for `onStatus`.

#### response[2] ####
> **Type:** Null
>
> The command object for this command, however `onStatus` has no such object and is always null.

#### response[3] ####
> **Type:** Object
>
> An object containing status information in response to a NetStream command. This contains at least the listed members, however more values may accompany a response as needed.
>
> #### response[3]["level"] ####
> > **Type:** String
> > 
> > The level of alert for this message. Usually "status", though it is "error" for errors which must be handled.
> >
> > Example value: "status"
> 
> #### response[3]["code"] ####
> > **Type:** String
> > 
> > The response code for the NetStream command. This varies based on the command which was issued.
> > 
> > Example value: "NetStream.Play.Start"
> 
> #### response[3]["description"]
> > **Type:** String
> > 
> > A human readable string which indicates what the status code should be interpreted by a human to mean.
> > 
> > Example value: "Connection successfully established."

## Possible Status Codes ##
#### NetStream.Buffer.Empty ####
> **Level:** "status"
> 
> The stream's buffers are starved, and playback will stall until the buffer is full again.

#### NetStream.Buffer.Full ####
> **Level:** "status"
> 
> The stream's buffer is full and will resume playback.

#### NetStream.Buffer.Flush ####
> **Level:** "status"
> 
> The stream has finished and remaining data is flushed from the buffers.

#### NetStream.Connect.Success ####
> **Level:** "status"
> 
> Sent when a NetStream is established.

#### NetStream.Connect.Failed ####
> **Level:** "error"
> 
> The NetStream failed for some reason.

#### NetStream.Failed ####
> **Level:** "error"
>
> The NetStream failed for some reason.

#### NetStream.Pause.Notify ####
> **Level:** "status"
> 
> The stream was paused by the viewer.

#### NetStream.Play.Failed ####
> **Level:** "error"
> 
> The stream could not be played for some reason.

#### NetStream.Play.PublishNotify ####
> **Level:** "status"
> 
> Publishing has begun on this stream.

#### NetStream.Play.Reset ####
> **Level:** "status"
> 
> All pending play commands have been flushed.

#### NetStream.Play.Start ####
> **Level:** "status"
> 
> Playback of the stream has started.

#### NetStream.Play.Stop ####
> **Level:** "status"
> 
> Playback of the stream has been stopped.

#### NetStream.Play.StreamNotFound ####
> **Level:** "status"
> 
> The stream could not be found.

#### NetStream.Play.UnpublishNotify ####
> **Level:** "status"
> 
> The publishing for this stream has halted.

#### NetStream.Publish.BadName ####
> **Level:** "error"
> 
> The stream publish attempt failed because someone is already publishing to the target stream.

#### NetStream.Publish.Failed ####
> **Level:** "error"
> 
> The stream publish attempt failed for some reason.

#### NetStream.Publish.Idle ####
> **Level:** "status"
> 
> The publisher has been idle for too long.

#### NetStream.Publish.Start ####
> **Level:** "status"
> 
> The publish attempt succeeded.

#### NetStream.Record.DiskQuotaExceeded ####
> **Level:** "error"
> 
> An attempt to record failed due to disk space quota being exhausted.

#### NetStream.Record.Failed ####
> **Level:** "error"
> 
> An attempt to record a stream failed for some reason.

#### NetStream.Record.NoAccess ####
> **Level:** "status"
> 
> An attempt was made to record to a read-only stream.

#### NetStream.Record.Start ####
> **Level:** "status"
> 
> A recording attempt was successfully started.

#### NetStream.Record.Stop ####
> **Level:** "status"
> 
> A recording in progress was stopped.

#### NetStream.Seek.InvalidTime ####
> **Level:** "error"
> 
> The user attempted to seek to a time which is outside of the bounds of the video, or is past the end of what has already been downloaded, though would otherwise be a valid time in the completely downloaded video. The `details` property contains the last valid position which can be seeked to.

#### NetStream.Seek.Failed ####
> **Level:** "error"
> 
> Alias for `NetStream.Seek.InvalidTime`

#### NetStream.Seek.Notify ####
> **Level:** "status"
> 
> The seek operation completed successfully.

#### NetStream.Unpause.Notify ####
> **Level:** "status"
> 
> The viewer has unpaused the stream.

#### NetStream.Unpublish.Success ####
> **Level:** "status"
> 
> An attempt to halt publishing was successful.

