# Measurements

You can measure the waveform with the mouse or with cursors. A cursor is a vertical line at a time in the capture.

## Measure a pulse with the pointer

Put the pointer on a pulse of a channel. A box near the pointer shows these values for the pulse:

- **Width**: the time length of the pulse.
- **Period**: the time from one edge to the next edge of the same direction.
- **Frequency**: 1 divided by the period.
- **Duty Cycle**: the high time as a percentage of the period.

![The measurement at the pointer](../figures/hover-measurement.png)

To turn this box on or off, open the measurement dock and use **Enable floating measurement**.

## Count the edges in an area

1. Put the pointer on the waveform of the channel, between the high level and the low level.
2. Move the pointer to the start of the area.
3. Click the left mouse button.
4. Move the pointer to the end of the area. The app shows the number of edges, rising edges and falling edges.
5. Click the left mouse button again to complete the measurement.

## Measure the time between two edges

1. Put the pointer on the first edge.
2. Click the left mouse button.
3. Move the pointer to the second edge. The app shows the time and the number of samples between the two edges.
4. Click the left mouse button again to complete the measurement.

![The time between two edges](../figures/edge-distance.png)

## Add a cursor

Use one of these methods:

- In the waveform area, double-click the left mouse button at the time that you want. If the pointer is near an edge, the cursor moves to the edge.
- In the time ruler, click the left mouse button. An arrow shows on the ruler. Click the arrow to add a cursor.

![Add a cursor from the time ruler](../figures/ruler-insert-cursor.png)

Each cursor has a number. The numbers start at 1.

## Move a cursor

Use one of these methods:

- Put the pointer on the cursor. The cursor line becomes thicker. Click the cursor. Move the mouse. Click again to release the cursor. Near an edge, the cursor moves to the edge.
- In the time ruler, click the left mouse button at the new time. The ruler shows the numbers of all the cursors. Click the number of the cursor that you want to move.

![Move a cursor from the time ruler](../figures/ruler-move-cursor.png)

## Go to a cursor

1. In the time ruler, click the right mouse button. The ruler shows the numbers of all the cursors.
2. Click the number of a cursor. The waveform moves to the position of that cursor.

![Go to cursor 3](../figures/ruler-jump-cursor.png)

## Measure with cursors

To open the measurement dock, click **Measure** on the toolbar or press `M`. The dock has these groups:

- **Cursor Distance**: the time and the number of samples between two cursors.
- **Edges**: the number of edges on one channel between two cursors.
- **Cursors**: the time and the sample number of each cursor.

To add a time measurement, do these steps:

1. In the **Cursor Distance** group, click the **+** button.
2. Click the start field and select the first cursor.
3. Click the end field and select the second cursor.

The dock shows the result in the **Time/Samples** column.

To add an edge count, do these steps:

1. In the **Edges** group, click the **+** button.
2. Select the start cursor and the end cursor.
3. Select the channel.

The dock shows the number of rising edges, falling edges and all edges.

To remove a measurement, click the **×** button on its row.

## Delete a cursor

Use one of these methods:

- Click the **×** on the cursor label in the time ruler.
- Click the **×** button of the cursor in the **Cursors** group of the measurement dock.

The app gives new numbers to the cursors that stay.
