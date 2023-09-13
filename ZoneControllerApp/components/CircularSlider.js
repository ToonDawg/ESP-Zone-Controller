import React, { PureComponent } from 'react'
import Svg, { Path, Defs, LinearGradient, Stop, Circle } from 'react-native-svg'
import { StyleSheet, View, PanResponder } from 'react-native'

export default class CircularSlider extends PureComponent {
    static defaultProps = {
        radius: 100, 
        strokeWidth: 20, 
        openingRadian: Math.PI / 4, 
        backgroundTrackColor: '#e8e8e8', 
        linearGradient: [{ stop: '0%', color: '#1890ff' }, { stop: '100%', color: '#f5222d' }], 
        min: 0, 
        max: 100, 
        buttonRadius: 12, 
        buttonBorderColor: '#fff', 
        buttonStrokeWidth: 1, 
    }

    constructor(props) {
        super(props)
        this._panResponder = PanResponder.create({
            onStartShouldSetPanResponder: () => true,
            onMoveShouldSetPanResponder: () => false,
            onPanResponderGrant: this._handlePanResponderGrant,
            onPanResponderMove: this._handlePanResponderMove,
            onPanResponderRelease: this._handlePanResponderEnd,
            onPanResponderTerminationRequest: () => false,
            onPanResponderTerminate: this._handlePanResponderEnd,
        })

        this.state = {
            value: props.value,
            isDragging: false
        };


        this._containerRef = React.createRef()
    }

    _handlePanResponderGrant = () => {
        this.setState({ isDragging: true });
        const { value } = this.state
        this._moveStartValue = value
        this._moveStartRadian = this.getRadianByValue(value)
        this._startCartesian = this.polarToCartesian(this._moveStartRadian)
    };

    _handlePanResponderMove = (e, gestureState) => {
        const { min, max, step, openingRadian } = this.props
        let { x, y } = this._startCartesian
        x += gestureState.dx
        y += gestureState.dy
        const radian = this.cartesianToPolar(x, y) 
        const ratio = (this._moveStartRadian - radian) / ((Math.PI - openingRadian) * 2) 
        const diff = max - min 

        let value
        if (step) {
            value = this._moveStartValue + Math.round(ratio * diff / step) * step
        } else {
            value = this._moveStartValue + ratio * diff
        }
        value = Math.max(
            min,
            Math.min(max, value),
        )
        this.setState(({ value: curValue }) => {
            value = Math.abs(value - curValue) > diff / 4 ? curValue : value 
            return { value: Math.round(value) }
        })
        this._fireChangeEvent('onChange');
    }


    _handlePanResponderEnd = (e, gestureState) => {
        this.setState({ isDragging: false });

        if (this.props.disabled) {
            return;
        }

        this._fireChangeEvent('onComplete');
    }

    _fireChangeEvent = event => {
        if (this.props[event]) {
            this.props[event](this.state.value);
        }
    };

    /**
     * @param {number} radian 
     */
    polarToCartesian(radian) {
        const { radius } = this.props
        const distance = radius + this._getExtraSize() / 2 
        const x = distance + radius * Math.sin(radian)
        const y = distance + radius * Math.cos(radian)
        return { x, y }
    }

    /**
     * 
     * @param {*} x 
     * @param {*} y 
     */
    cartesianToPolar(x, y) {
        const { radius } = this.props
        const distance = radius + this._getExtraSize() / 2 
        if (x === distance) {
            return y > distance ? 0 : Math.PI / 2
        }
        const a = Math.atan((y - distance) / (x - distance)) 
        return (x < distance ? Math.PI * 3 / 2 : Math.PI / 2) - a
    }


    getCurrentRadian() {
        return this.getRadianByValue(this.state.value)
    }


    getRadianByValue(value) {
        const { openingRadian, min, max } = this.props
        return (Math.PI - openingRadian) * 2 * (max - value) / (max - min) + openingRadian
    }

    _getExtraSize() {
        const { strokeWidth, buttonRadius, buttonStrokeWidth } = this.props
        return Math.max(strokeWidth, (buttonRadius + buttonStrokeWidth) * 2)
    }

    _onLayout = () => {
        const ref = this._containerRef.current
        if (ref) {
            ref.measure((x, y, width, height, pageX, pageY) => {
                this.vertexX = pageX
                this.vertexY = pageY
            })
        }
    }

    interpolateColor(color1, color2, factor) {
        if (arguments.length < 3) { 
            factor = 0.5; 
        }
        var result = color1.slice();
        for (var i = 0; i < 3; i++) {
            result[i] = Math.round(result[i] + factor * (color2[i] - color1[i]));
        }
        return result;
    };
    
    getColorForPercentage(pct) {
        const zero = [0, 128, 255];  // RGB for blue
        const full = [255, 165, 0]; // RGB for orange
        return interpolateColor(zero, full, pct);
    }
    

    static getDerivedStateFromProps(nextProps, prevState) {
        if (!prevState.isDragging && nextProps.value !== prevState.value) {
            return { value: nextProps.value };
        }
        return null;
    }
    



    render() {
        const {
            radius,
            strokeWidth,
            backgroundTrackColor,
            openingRadian,
            linearGradient,
            buttonRadius,
            buttonBorderColor,
            buttonFillColor,
            buttonStrokeWidth,
            style,
            contentContainerStyle,
            children
        } = this.props
        const svgSize = radius * 2 + this._getExtraSize()
        const startRadian = 2 * Math.PI - openingRadian 
        const startPoint = this.polarToCartesian(startRadian)
        const endPoint = this.polarToCartesian(openingRadian)
        const currentRadian = this.getCurrentRadian() 
        const curPoint = this.polarToCartesian(currentRadian)

        const contentStyle = [
            styles.content,
            contentContainerStyle
        ]

        return (
            <View onLayout={this._onLayout} ref={this._containerRef} style={[styles.container, style]}>
                <View style={contentStyle}>
                    {children}
                </View>
                <Svg width={svgSize} height={svgSize}>
                    <Defs>
                        <LinearGradient
                            x1="0%"
                            y1="100%"
                            x2="100%"
                            y2="0%"
                            id="gradient">
                            {
                                linearGradient.map((item, index) => (
                                    <Stop
                                        key={index}
                                        offset={item.stop}
                                        stopColor={item.color}
                                    />
                                ))
                            }
                        </LinearGradient>
                    </Defs>
                    <Path
                        strokeWidth={strokeWidth}
                        stroke={backgroundTrackColor}
                        fill="none"
                        strokeLinecap="round"
                        d={`M${startPoint.x},${startPoint.y} A ${radius},${radius},0,${startRadian - openingRadian >= Math.PI ? '1' : '0'},1,${endPoint.x},${endPoint.y}`}
                    />
                    <Path
                        strokeWidth={strokeWidth}
                        stroke="url(#gradient)"
                        fill="none"
                        strokeLinecap="round"
                        d={`M${startPoint.x},${startPoint.y} A ${radius},${radius},0,${startRadian - currentRadian >= Math.PI ? '1' : '0'},1,${curPoint.x},${curPoint.y}`}
                    />
                    <Circle
                        cx={curPoint.x}
                        cy={curPoint.y}
                        r={buttonRadius}
                        fill={buttonFillColor || buttonBorderColor}
                        stroke={buttonBorderColor}
                        strokeWidth={buttonStrokeWidth}
                        {...this._panResponder.panHandlers}
                    />
                </Svg>
            </View>
        )
    }
}

const styles = StyleSheet.create({
    container: {
        justifyContent: 'center',
        alignItems: 'center'
    },
    content: {
        position: 'absolute',
        left: 0,
        top: 0,
        bottom: 0,
        right: 0
    }
})
