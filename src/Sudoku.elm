port module Sudoku exposing (..)

import Browser
import Html exposing (Html)
import Html.Events exposing (onClick, onInput)
import Html.Attributes as Attr
import Array exposing (Array)


-- MAIN

main : Program BoardSize Model Msg
main =
    Browser.element {
        init = init,
        update = update,
        view = view,
        subscriptions = subscriptions }


-- PORTS

port fetchBoard : BoardSize -> Cmd msg
port recvBoard : (Board -> msg) -> Sub msg
port recvHash : (String -> msg) -> Sub msg


-- MODEL

type alias BoardSize = String
type alias BoardData = List (List String)
type alias Board = {data: BoardData, size: BoardSize}
type alias Model = {
    board: Board,
    selectedSize: BoardSize,
    showSolution: Bool }


init : BoardSize -> (Model, Cmd Msg)
init boardSize =
    let
        emptyBoard = Board [] "0x0"
        emptyModel = Model emptyBoard boardSize False
    in
        ( emptyModel, fetchBoard boardSize )


-- UPDATE

type Msg
    = ShowSolution
    | HideSolution
    | RecvBoard Board
    | RecvHash String
    | RequestBoard
    | SelectSize String


update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case msg of
        ShowSolution ->
            ({ model | showSolution = True }, Cmd.none)

        HideSolution ->
            ({ model | showSolution = False }, Cmd.none)

        SelectSize size ->
            ({ model | selectedSize = size }, Cmd.none)

        RecvBoard board ->
            ({ model | board = board, selectedSize = board.size}, Cmd.none)

        RecvHash hash ->
            let
                size = String.dropLeft 1 hash
            in
                (model, if size == model.board.size then Cmd.none else fetchBoard size)

        RequestBoard ->
            (model, fetchBoard model.selectedSize)


-- VIEW

view : Model -> Html Msg
view model =
    Html.div []
        [ viewSizeSelect model
        , viewBoard model
        , viewShowButton model
        ]

viewSizeSelect : Model -> Html Msg
viewSizeSelect model =
    let
        defaultChoices = ["2x2", "3x3", "4x4"]

        choices =
            if List.member model.selectedSize defaultChoices then
                defaultChoices
            else
                model.selectedSize :: defaultChoices
            |> List.sort

        sizeOption v =
            Html.option
                [ Attr.value v, Attr.selected (model.selectedSize == v) ]
                [ Html.text v ]
    in
        Html.div []
            [ Html.text "Size:"
            , Html.select
                [ onInput SelectSize
                , Attr.name "size"
                ] (List.map sizeOption choices)
            , Html.input
                [ onClick RequestBoard
                , Attr.type_ "submit"
                , Attr.value "Go!"
                ] []
            ]


viewShowButton : Model -> Html Msg
viewShowButton model =
    case model.showSolution of
        True ->
            Html.button [ onClick HideSolution ] [ Html.text "Hide solution" ]

        False ->
            Html.button [ onClick ShowSolution ] [ Html.text "Show solution" ]


viewBoard : Model -> Html Msg
viewBoard model =
    let
        sizeParts = List.map String.toInt (String.split "x" model.board.size)

        (n0, n1) =
            case sizeParts of
                [Just a, Just b] ->
                    (a, b)
                _ ->
                    (3, 3)

        n = n0 * n1

        cellAttrs i j =
            [ Attr.classList
                [ ("top", i > 0 && modBy n1 i == 0)
                , ("bottom", i < n && modBy n1 (i+1) == 0)
                , ("left", j > 0 && modBy n0 j == 0)
                , ("right", j < n && modBy n0 (j+1) == 0)
                ]
            ]

        cellTextAttrs s =
            if String.startsWith "~" s then
                if model.showSolution then
                    [ Attr.class "shown" ]
                else
                    [ Attr.class "hidden" ]
            else
                []

        cellText s =
            if String.startsWith "~" s then
                String.dropLeft 1 s
            else
                s

        formatCell : Int -> Int -> String -> Html Msg
        formatCell row col s =
            Html.td
                ( cellAttrs row col )
                [ Html.span
                    ( cellTextAttrs s )
                    [ Html.text (cellText s) ]
                ]

        formatRow : Int -> List String -> Html Msg
        formatRow row items =
            Html.tr [] ( List.indexedMap (formatCell row) items )
    in
        Html.table
            [ Attr.class "board" ]
            ( List.indexedMap formatRow model.board.data )


-- Subscriptions

subscriptions : Model -> Sub Msg
subscriptions _ =
    Sub.batch
        [ recvBoard RecvBoard
        , recvHash RecvHash
        ]
