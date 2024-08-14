"use client";
import "leaflet/dist/leaflet.css";
import "../styles/Home.css";

import { Box } from "@radix-ui/themes";

import { MapContainer } from "react-leaflet/MapContainer";
import { Marker } from "react-leaflet/Marker";
import { Popup } from "react-leaflet/Popup";
import { TileLayer } from "react-leaflet/TileLayer";
import { LayersControl } from "react-leaflet";
import type { IconOptions, LatLngTuple } from "leaflet";
import L from "leaflet";

// import { Polyline} from "react-leaflet";

function CarMap() {
	const carIconOptions: IconOptions = {
		iconUrl: "car-marker.png",
		iconSize: [64.5, 38.7], //645, 387
		iconAnchor: [22, 94],
		popupAnchor: [-3, -76],
	};
	const mineIconOptions: IconOptions = {
		iconUrl: "mine-marker.png",
		iconSize: [64.5, 38.7], //645, 387
		iconAnchor: [22, 94],
		popupAnchor: [-3, -76],
	};
	const anomalies: LatLngTuple[] = [
		[9.53709, 6.46616],
		[9.53708, 6.46587],
		[9.53704, 6.46561],
		[9.53703, 6.46549],
		[9.537, 6.46523],
		[9.53696, 6.46452],
		[9.53697, 6.46437],
		[9.5369, 6.4637],
		[9.53775, 6.45959],
		[9.53779, 6.45947],
		[9.53744, 6.4562],
		[9.53734, 6.45604],
		[9.53732, 6.45602],
		[9.53705, 6.45567],
		[9.53647, 6.45452],
		[9.53625, 6.45444],
		[9.53629, 6.45477],
		[9.53721, 6.46071],
		[9.5368, 6.46428],
		[9.53691, 6.46473],
		[9.53695, 6.46519],
		[9.53703, 6.4672],
	];
	const carIcon = L.icon(carIconOptions);
	const mineIcon = L.icon(mineIconOptions);
	return (
		<MapContainer
			className="map"
			center={[9.53405, 6.45567]}
			zoom={17}
			scrollWheelZoom={false}
			zoomAnimation={true}
		>
			<TileLayer
				attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
				url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
			/>
			{/* <Marker position={[51.505, -0.09]}>
    				<Popup>
    					A pretty CSS3 popup. <br /> Easily customizable.
    				</Popup>
    			</Marker> */}
			{/* <Polyline positions={[]}/> */}
			<LayersControl>
				<LayersControl.Overlay checked name="Car Layer">
					<Marker position={[9.53682, 6.45959]} icon={carIcon}>
						<Popup>Car Marker</Popup>
					</Marker>
				</LayersControl.Overlay>
				<LayersControl.Overlay checked name="Anomalies">
					{anomalies.map((anomaly, i) => (
						<Marker position={[anomaly[0] -0.0007, anomaly[1]]} key={i} icon={mineIcon} zIndexOffset={0}>
							<Popup>Car Marker</Popup>
						</Marker>
					))}
				</LayersControl.Overlay>
			</LayersControl>
		</MapContainer>
	);
}

export default CarMap;
